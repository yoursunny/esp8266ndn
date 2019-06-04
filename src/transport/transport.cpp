#include "transport.hpp"
#include "../core/logger.hpp"

#define TRANSPORT_DBG(...) DBG(Transport, __VA_ARGS__)

namespace ndn {

Transport::~Transport() = default;

void
Transport::loop()
{
  if (m_rxCb == nullptr) {
    return;
  }

  PacketBuffer* pb = nullptr;
  bool ok = false;
  for (std::tie(pb, ok) = m_rxQueueOut.pop(); ok; std::tie(pb, ok) = m_rxQueueOut.pop()) {
    --m_nRxBufs;
    m_rxCb(m_rxCbArg, pb);
  }
}

void
Transport::onReceive(ReceiveCallback cb, void* cbarg)
{
  m_rxCb = cb;
  m_rxCbArg = cbarg;
}

void
Transport::pushReceiveBuffer(PacketBuffer* pb)
{
  if (m_rxQueueIn.push(pb)) {
    ++m_nRxBufs;
  }
  else {
    TRANSPORT_DBG("rxQueueIn is full");
    delete pb;
  }
}

PacketBuffer*
Transport::beforeReceive()
{
  PacketBuffer* pb = nullptr;
  bool ok = false;
  std::tie(pb, ok) = m_rxQueueIn.pop();
  if (!ok) {
    TRANSPORT_DBG("rxQueueIn is empty");
    return nullptr;
  }
  return pb;
}

void
Transport::afterReceive(PacketBuffer* pb, size_t pktSize, bool isAsync)
{
  if (pktSize == 0 || m_rxCb == nullptr) {
    pushReceiveBuffer(pb);
    return;
  }

  ndn_Error e = pb->parse(pktSize);
  if (e != NDN_ERROR_success) {
    TRANSPORT_DBG("packet parse error " << e << " endpoint=" << pb->endpointId);
    pushReceiveBuffer(pb);
    return;
  }

  if (isAsync) {
    if (!m_rxQueueOut.push(pb)) {
      TRANSPORT_DBG("rxQueueOut is full");
      pushReceiveBuffer(pb);
    }
  }
  else {
    --m_nRxBufs;
    m_rxCb(m_rxCbArg, pb);
  }
}

void
PollModeTransport::loop()
{
  Transport::loop();
  PacketBuffer* pb = beforeReceive();
  if (pb == nullptr) {
    return;
  }
  uint8_t* buf = nullptr;
  size_t bufSize = 0;
  std::tie(buf, bufSize) = pb->useBuffer();
  size_t pktSize = receive(buf, bufSize, pb->endpointId);
  afterReceive(pb, pktSize, false);
}

} // namespace ndn
