#include "unix-time.hpp"
#include "../core/logger.hpp"

#include <Arduino.h>
#include <cstdio>
#include <sys/time.h>
#include <time.h>

#define LOG(...) LOGGER(UnixTime, __VA_ARGS__)

#if defined(ARDUINO_ARCH_ESP32)
#define HAVE_SYSCLOCK_INTEG
#endif

namespace esp8266ndn {

void
UnixTimeClass::disableIntegration()
{
  m_timeOffset = TIMEOFFSET_UNAVAIL;
}

bool
UnixTimeClass::begin(ndnph::Face& face, int interval)
{
  if (!face.addHandler(*this)) {
    return false;
  }

  m_interval = std::max(interval, 5000);

  static ndnph::StaticRegion<512> region;
  region.reset();
  m_interest = region.create<ndnph::Interest>();
  m_interest.setName(ndnph::Name::parse(region, "/localhop/unix-time"));
  m_interest.setCanBePrefix(true);
  m_interest.setMustBeFresh(true);
  return true;
}

void
UnixTimeClass::loop()
{
  auto now = millis();
  if (ndnph::port::Clock::isBefore(now, m_nextRequest)) {
    return;
  }

  if (m_lastRequest != 0) {
    LOG(F("last-request=") << m_lastRequest << F(" timeout"));
  }

  send(m_interest);
  m_lastRequest = now;
  m_nextRequest = now + m_interval;
  LOG(F("requesting-at=") << m_lastRequest << F(", next-request=") << m_nextRequest);
}

bool
UnixTimeClass::isAvailable() const
{
  return m_timeOffset != TIMEOFFSET_UNAVAIL && m_timeOffset != TIMEOFFSET_INTEG_UNAVAIL;
}

uint64_t
UnixTimeClass::now() const
{
  if (!isAvailable()) {
    return 0;
  }

#ifdef HAVE_SYSCLOCK_INTEG
  if (m_timeOffset == TIMEOFFSET_INTEG) {
    struct timeval tv = { 0 };
    gettimeofday(&tv, nullptr);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
  }
#endif

  return m_timeOffset + millis() * 1000;
}

bool
UnixTimeClass::processData(ndnph::Data data)
{
  const ndnph::Name& name = data.getName();
  if (name.size() != m_interest.getName().size() + 1 || !m_interest.getName().isPrefixOf(name) ||
      !name[-1].is<ndnph::convention::Timestamp>()) {
    return false;
  }
  uint64_t timestamp = name[-1].as<ndnph::convention::Timestamp>();

  if (m_lastRequest == 0) {
    LOG(F("ignore-no-request"));
    return true;
  }
  auto now = millis();
  auto rtt = now - m_lastRequest;
  if (rtt > 1000) {
    LOG(F("ignore-high-rtt=") << rtt);
    return true;
  }

#ifdef HAVE_SYSCLOCK_INTEG
  if (m_timeOffset == TIMEOFFSET_INTEG_UNAVAIL) {
    m_timeOffset = TIMEOFFSET_INTEG;
  }
#endif

  // Calculate timeOffset from lastRequest only, because the face transmits the Interest
  // immediately, but processes incoming Data in Face::loop() that incurs delay from
  // Arduino's main loop.
  uint64_t timeOffset = timestamp - m_lastRequest * 1000;
  uint64_t unixNow = timeOffset + now * 1000;
#ifdef HAVE_SYSCLOCK_INTEG
  if (m_timeOffset == TIMEOFFSET_INTEG) {
    struct timeval tv = {
      .tv_sec = static_cast<time_t>(unixNow / 1000000),
      .tv_usec = static_cast<suseconds_t>(unixNow % 1000000),
    };
    settimeofday(&tv, nullptr);
  } else
#endif
  {
    m_timeOffset = timeOffset;
  }

  LOG(F("time-offset=") << timeOffset << F(", now=") << unixNow);
  m_lastRequest = 0;
  return true;
}

String
UnixTimeClass::toRfc3399DateTime(uint64_t timestamp)
{
  time_t sec = timestamp / 1000000;
  int usec = static_cast<int>(timestamp % 1000000);

  struct tm t;
  if (gmtime_r(&sec, &t) == nullptr) {
    return "";
  }

  char buf[32];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%06dZ", 1900 + t.tm_year, 1 + t.tm_mon,
           t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, usec);
  return buf;
}

UnixTimeClass UnixTime;

} // namespace esp8266ndn
