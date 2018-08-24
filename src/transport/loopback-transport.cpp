#include "loopback-transport.hpp"
#include "../core/logger.hpp"

#define LOOPBACKTRANSPORT_DBG(...) DBG(LoopbackTransport, __VA_ARGS__)

namespace ndn {

LoopbackTransport::LoopbackTransport()
  : m_other(nullptr)
  , m_len(0)
{
}

void
LoopbackTransport::begin(LoopbackTransport& other)
{
  m_other = &other;
  other.m_other = this;
}

size_t
LoopbackTransport::receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId)
{
  if (m_len == 0) {
    return 0;
  }

  memcpy(buf, m_pkt, min(m_len, bufSize));
  endpointId = m_endpointId;

  size_t len = m_len;
  m_len = 0;
  return len;
}

ndn_Error
LoopbackTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  if (m_other == nullptr) {
    LOOPBACKTRANSPORT_DBG("cannot send without begin()");
    return NDN_ERROR_SocketTransport_socket_is_not_open;
  }

  if (m_other->m_len > 0) {
    LOOPBACKTRANSPORT_DBG("receiver is congested");
    return NDN_ERROR_SocketTransport_error_in_send;
  }

  m_other->m_len = min(len, static_cast<size_t>(LOOPBACKTRANSPORT_PKTSIZE));
  memcpy(m_other->m_pkt, pkt, m_other->m_len);
  m_other->m_endpointId = endpointId;
  return NDN_ERROR_success;
}

} // namespace ndn