#include "loopback-transport.hpp"
#include "../core/logger.hpp"

#define LOG(...) LOGGER(LoopbackTransport, __VA_ARGS__)

namespace ndn {

void
LoopbackTransport::begin(LoopbackTransport& other)
{
  m_other = &other;
  other.m_other = this;
}

ndn_Error
LoopbackTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  if (m_other == nullptr) {
    LOG(F("cannot send without begin()"));
    return NDN_ERROR_SocketTransport_socket_is_not_open;
  }

  PacketBuffer* pb = m_other->beforeReceive();
  if (pb == nullptr) {
    LOG(F("receiver is congested"));
    return NDN_ERROR_SocketTransport_error_in_send;
  }

  uint8_t* buf = nullptr;
  size_t bufSize = 0;
  std::tie(buf, bufSize) = pb->useBuffer();
  size_t pktSize = std::min(len, bufSize);
  memcpy(buf, pkt, pktSize);
  pb->endpointId = endpointId;
  return m_other->afterReceive(pb, pktSize, true);
}

} // namespace ndn