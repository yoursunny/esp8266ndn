#include "ble-client-transport.hpp"
#include "detail/ble-impl.hpp"
#include "../core/logger.hpp"

#define LOG(...) LOGGER(BleClientTransport, __VA_ARGS__)

namespace ndn {

BleClientTransport::BleClientTransport() = default;

BleClientTransport::~BleClientTransport() = default;

bool
BleClientTransport::begin()
{
  int err = detail::BleDeviceImpl.init("esp8266ndn-client", false, 1);
  if (err != 0) {
    LOG(F("device init error ") << err);
    return false;
  }

  m_impl.reset(new detail::BleClientImpl());
  if (!m_impl->begin()) {
    LOG(F("client begin failure"));
    m_impl.reset();
    return false;
  }

  err = detail::BleDeviceImpl.startScanConnect(*m_impl);
  if (err != 0) {
    LOG(F("device scan error ") << err);
    m_impl.reset();
    return false;
  }

  return true;
}

size_t
BleClientTransport::receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId)
{
  if (m_impl == nullptr) {
    return 0;
  }
  endpointId = 0;
  return m_impl->receive(buf, bufSize);
}

ndn_Error
BleClientTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  return m_impl != nullptr && m_impl->send(pkt, len) ?
         NDN_ERROR_success :
         NDN_ERROR_SocketTransport_socket_is_not_open;
}

} // namespace ndn
