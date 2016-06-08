#include "unicast-udp-transport.hpp"
#include "logger.hpp"

#define UUDPTRANSPORT_DBG(...) DBG(UnicastUdpTransport, __VA_ARGS__)

namespace ndn {

UnicastUdpTransport::UnicastUdpTransport(UDP& udp)
  : m_udp(udp)
  , m_routerIp(INADDR_NONE)
  , m_routerPort(0)
{
}

void
UnicastUdpTransport::begin(IPAddress routerIp, uint16_t routerPort, uint16_t localPort)
{
  m_routerIp = routerIp;
  m_routerPort = routerPort;
  m_udp.begin(localPort);
}

void
UnicastUdpTransport::end()
{
  m_routerIp = INADDR_NONE;
  m_routerPort = 0;
  m_udp.stop();
}

size_t
UnicastUdpTransport::receive(uint8_t* buf, size_t bufSize, uint64_t* endpointId)
{
  if (m_udp.parsePacket() <= 0) {
    return 0;
  }

  if (m_udp.remoteIP() != m_routerIp || m_udp.remotePort() != m_routerPort) {
    return 0;
  }

  int len = m_udp.read(buf, bufSize);
  if (len <= 0) {
    return 0;
  }

  *endpointId = 0;
  return static_cast<size_t>(len);
}

void
UnicastUdpTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  if (m_routerIp == INADDR_NONE) {
    UUDPTRANSPORT_DBG(F("cannot send without begin()"));
    return;
  }

  if (endpointId != 0) {
    UUDPTRANSPORT_DBG(F("cannot send to non-zero endpointId"));
    return;
  }

  int res = m_udp.beginPacket(m_routerIp, m_routerPort);
  if (res != 1) {
    UUDPTRANSPORT_DBG(F("Udp::beginPacket error"));
    return;
  }
  m_udp.write(pkt, len);
  res = m_udp.endPacket();
  if (res != 1) {
    UUDPTRANSPORT_DBG(F("Udp::endPacket error"));
    return;
  }
}

} // namespace ndn