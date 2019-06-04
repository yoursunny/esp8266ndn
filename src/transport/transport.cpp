#include "transport.hpp"
#include "../core/logger.hpp"

#define TRANSPORT_DBG(...) DBG(Transport, __VA_ARGS__)

namespace ndn {

Transport::~Transport() = default;

void
Transport::loop()
{
}

void
Transport::onReceive(ReceiveCallback cb, void* cbarg)
{
  m_rxCb = cb;
  m_rxCbArg = cbarg;
}

size_t
Transport::countReceiveBuffer() const
{
  return m_pb != nullptr ? 1 : 0;
}

void
Transport::pushReceiveBuffer(PacketBuffer* pb)
{
  // TODO store several receive buffers
  if (m_pb != nullptr) {
    delete m_pb;
  }
  m_pb = pb;
}

PacketBuffer*
Transport::beforeReceive()
{
  if (m_pb == nullptr) {
    TRANSPORT_DBG("no-receive-buffer");
  }
  PacketBuffer* pb = m_pb;
  m_pb = nullptr;
  return pb;
}

void
Transport::afterReceive(PacketBuffer* pb, size_t pktSize, uint64_t endpointId)
{
  if (pktSize == 0 || m_rxCb == nullptr) {
    pushReceiveBuffer(pb);
    return;
  }

  ndn_Error e = pb->parse(pktSize);
  if (e != NDN_ERROR_success) {
    TRANSPORT_DBG("malformed-packet endpoint=" << endpointId << " e=" << e);
    pushReceiveBuffer(pb);
    return;
  }

  m_rxCb(m_rxCbArg, pb, endpointId);
}

void
PollModeTransport::loop()
{
  PacketBuffer* pb = beforeReceive();
  if (pb == nullptr) {
    return;
  }
  uint8_t* buf = nullptr;
  size_t bufSize = 0;
  std::tie(buf, bufSize) = pb->useBuffer();
  uint64_t endpointId = 0;
  size_t pktSize = receive(buf, bufSize, endpointId);
  afterReceive(pb, pktSize, endpointId);
}

} // namespace ndn
