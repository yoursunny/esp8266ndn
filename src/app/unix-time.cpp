#include "unix-time.hpp"
#include "../core/logger.hpp"

#include <Arduino.h>
#include <cstdio>
#include <time.h>

#define LOG(...) LOGGER(UnixTime, __VA_ARGS__)

namespace ndn {

UnixTimeClass::UnixTimeClass()
  : m_interval(0)
  , m_lastRequest(0)
  , m_nextRequest(0)
  , m_timeOffset(0)
{
  ndn::NameLite& name = m_interest.getName();
  name.append("localhop");
  name.append("unix-time");
  m_interest.setCanBePrefix(true);
  m_interest.setMustBeFresh(true);
}

void
UnixTimeClass::begin(ndn::Face& face, unsigned long interval)
{
  m_interval = std::max<unsigned long>(interval, 5000);
  face.addHandler(this);
}

void
UnixTimeClass::loop()
{
  if (m_nextRequest > millis()) {
    return;
  }

  if (m_lastRequest != 0) {
    LOG(F("last-request=") << m_lastRequest << F(" timeout"));
  }

  getFace()->sendInterest(m_interest);
  m_lastRequest = millis();
  m_nextRequest = m_lastRequest + m_interval;
  LOG(F("requesting-at=") << m_lastRequest << F(", next-request=") << m_nextRequest);
}

bool
UnixTimeClass::isAvailable() const
{
  return m_timeOffset != 0;
}

uint64_t
UnixTimeClass::now() const
{
  if (!isAvailable()) {
    return 0;
  }

  return m_timeOffset + millis() * 1000;
}

bool
UnixTimeClass::processData(const ndn::DataLite& data, uint64_t endpointId)
{
  const ndn::NameLite& name = data.getName();
  uint64_t timestamp;
  if (!m_interest.getName().match(name) ||
      name.size() != m_interest.getName().size() + 1 ||
      name.get(-1).toTimestamp(timestamp) != NDN_ERROR_success) {
    return false;
  }

  if (m_lastRequest == 0) {
    LOG(F("ignore-no-request"));
    return true;
  }
  auto rtt = millis() - m_lastRequest;
  if (rtt > 1000) {
    LOG(F("ignore-high-rtt=") << rtt);
    return true;
  }

  // Calculate timeOffset from lastRequest only, because Face usually transmits
  // the Interest immediately, but processes incoming Data in Face::loop that
  // incurs delay from Arduino's main loop.
  m_timeOffset = timestamp - m_lastRequest * 1000;
  LOG(F("time-offset=") << m_timeOffset << F(", now=") << this->now());
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
  snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%06dZ",
           1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday,
           t.tm_hour, t.tm_min, t.tm_sec, usec);
  return buf;
}

UnixTimeClass UnixTime;

} // namespace ndn
