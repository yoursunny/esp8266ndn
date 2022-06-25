#include "ble-server-transport.hpp"
#include "../core/logger.hpp"
#include "ble-uuid.hpp"

#define LOG(...) LOGGER(BleServerTransport, __VA_ARGS__)

namespace esp8266ndn {

BleServerTransportBase::BleServerTransportBase(size_t mtu)
  : DynamicRxQueueMixin(mtu)
{}

void
BleServerTransportBase::handleReceive(const uint8_t* pkt, size_t pktLen, uint64_t endpointId)
{
  auto r = receiving();
  if (!r) {
    LOG(F("drop: no RX buffer"));
    return;
  }

  if (pktLen > r.bufLen()) {
    LOG(F("drop: RX buffer too short, pktLen=") << _DEC(pktLen));
    return;
  }

  std::copy_n(pkt, pktLen, r.buf());
  r(pktLen, endpointId);
}

void
BleServerTransportBase::doLoop()
{
  loopRxQueue();
}

#ifdef ESP8266NDN_HAVE_ESP32BLE
void
BleServerTransport::warnBluedroid()
{
  LOG(F("using ESP32 Bluedroid stack that has limited compatibility; "
        "it's recommended to switch to NimBLE stack"));
}
#endif

} // namespace esp8266ndn
