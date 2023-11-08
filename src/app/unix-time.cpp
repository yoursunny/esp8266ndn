#include "unix-time.hpp"
#include "../core/logger.hpp"

#define LOG(...) LOGGER(UnixTime, __VA_ARGS__)

namespace esp8266ndn {

static const ndnph::Name&
getLocalhopUnixTimePrefix() {
  static const uint8_t tlv[]{
    0x08, 0x08, 0x6C, 0x6F, 0x63, 0x61, 0x6C, 0x68, 0x6F, 0x70,       // localhop
    0x08, 0x09, 0x75, 0x6E, 0x69, 0x78, 0x2D, 0x74, 0x69, 0x6D, 0x65, // unix-time
  };
  static const ndnph::Name name(tlv, sizeof(tlv));
  return name;
}

UnixTime::UnixTime(ndnph::Face& face)
  : PacketHandler(face)
  , m_pending(this) {}

void
UnixTime::begin(int interval) {
  m_interval = std::max(5000, interval);
  m_lastRequest = ndnph::port::Clock::now();
  m_nextRequest = m_lastRequest;
}

void
UnixTime::loop() {
  if (m_interval == 0) {
    return;
  }
  auto now = ndnph::port::Clock::now();
  if (ndnph::port::Clock::isBefore(now, m_nextRequest)) {
    return;
  }

  m_lastRequest = now;
  m_nextRequest = ndnph::port::Clock::add(now, m_interval);

  ndnph::StaticRegion<512> region;
  auto interest = region.create<ndnph::Interest>();
  assert(!!interest);
  interest.setName(getLocalhopUnixTimePrefix());
  interest.setCanBePrefix(true);
  interest.setMustBeFresh(true);
  m_pending.send(interest);

  LOG(F("send-request"));
}

bool
UnixTime::processData(ndnph::Data data) {
  auto name = data.getName();
  if (!m_pending.match(data, getLocalhopUnixTimePrefix()) ||
      name.size() != getLocalhopUnixTimePrefix().size() + 1 ||
      !name[-1].is<ndnph::convention::Timestamp>()) {
    return false;
  }

  uint64_t timestamp = name[-1].as<ndnph::convention::Timestamp>();

  auto now = ndnph::port::Clock::now();
  auto rtt = ndnph::port::Clock::sub(now, m_lastRequest);
  if (rtt > 1000) {
    LOG(F("ignore-high-rtt=") << rtt);
    return true;
  }

  // Calculate timeOffset from m_lastRequest only, because Interest transmission is instantaneous
  // but Data processing is delayed in from Arduino's main loop.
  uint64_t unixNow = timestamp + rtt * 1000;
  ndnph::port::UnixTime::set(unixNow);
  LOG(F("now=") << unixNow);
  return true;
}

} // namespace esp8266ndn
