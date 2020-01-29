#include "ble-server-transport.hpp"
#include "detail/ble-impl.hpp"
#include "../core/logger.hpp"

#define LOG(...) LOGGER(BleServerTransport, __VA_ARGS__)

namespace ndn {

BleServerTransport::BleServerTransport() = default;

BleServerTransport::~BleServerTransport() = default;

bool
BleServerTransport::begin(const char* deviceName)
{
  int err = detail::BleDeviceImpl.init(deviceName, true, 0);
  if (err != 0) {
    LOG(F("device init error ") << err);
    return false;
  }

  m_impl.reset(new detail::BleServiceImpl());
  err = m_impl->begin();
  if (err != 0) {
    LOG(F("service begin error ") << err);
    m_impl.reset();
    return false;
  }

  err = m_impl->advertise();
  if (err != 0) {
    LOG(F("advertise error ") << err);
    m_impl.reset();
    return false;
  }

  LOG(F("Server ready at ") << detail::BleDeviceImpl.getAddr());
  return true;
}

size_t
BleServerTransport::receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId)
{
  if (m_impl == nullptr) {
    return 0;
  }
  endpointId = 0;
  return m_impl->receive(buf, bufSize);
}

ndn_Error
BleServerTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  return m_impl != nullptr && m_impl->send(pkt, len) ?
         NDN_ERROR_success :
         NDN_ERROR_SocketTransport_socket_is_not_open;
}

} // namespace ndn
