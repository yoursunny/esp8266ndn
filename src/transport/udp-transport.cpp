#include "udp-transport.hpp"
#include "../core/logger.hpp"

#include <WiFiUdp.h>

#define UDPTRANSPORT_DBG(...) DBG(UdpTransport, __VA_ARGS__)

namespace ndn {

static_assert(sizeof(UdpTransport::EndpointId) == sizeof(uint64_t), "");

const IPAddress UdpTransport::MCAST_GROUP(224, 0, 23, 170);

UdpTransport::UdpTransport(UDP& udp)
  : m_udp(udp)
  , m_remoteIp(INADDR_NONE)
  , m_remotePort(0)
{
}

bool
UdpTransport::begin(uint16_t localPort)
{
  UDPTRANSPORT_DBG(F("listening on port ") << _DEC(localPort));
  return m_udp.begin(localPort) == 1;
}

void
UdpTransport::end()
{
  m_remoteIp = INADDR_NONE;
  m_remotePort = 0;
  m_udp.stop();
}

void
UdpTransport::connect(IPAddress remoteIp, uint16_t remotePort)
{
  UDPTRANSPORT_DBG(F("connected with ") << remoteIp << ':' << remotePort);
  m_remoteIp = remoteIp;
  m_remotePort = remotePort;
}

size_t
UdpTransport::receive(uint8_t* buf, size_t bufSize, uint64_t* endpointId)
{
  if (m_udp.parsePacket() <= 0) {
    return 0;
  }

  if (m_remotePort != 0) {
    if (m_udp.remoteIP() != m_remoteIp || m_udp.remotePort() != m_remotePort) {
      m_udp.flush();
      return 0;
    }
  }

  int len = m_udp.read(buf, bufSize);
  m_udp.flush();
  if (len <= 0) {
    return 0;
  }

  EndpointId endpoint = {0};
  if (m_remotePort == 0) {
    endpoint.ip = static_cast<uint32_t>(m_udp.remoteIP());
    endpoint.port = m_udp.remotePort();
  }
  *endpointId = endpoint.endpointId;
  return static_cast<size_t>(len);
}

ndn_Error
UdpTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  EndpointId endpoint;
  endpoint.endpointId = endpointId;

  int res = -1;
  if (endpointId == 0) {
    if (m_remotePort == 0) {
      UDPTRANSPORT_DBG(F("remote endpoint not specified"));
      return NDN_ERROR_SocketTransport_error_in_getaddrinfo;
    }
    res = m_udp.beginPacket(m_remoteIp, m_remotePort);
  }
  else {
    res = m_udp.beginPacket(IPAddress(endpoint.ip), endpoint.port);
  }

  if (res != 1) {
    UDPTRANSPORT_DBG(F("Udp::beginPacket error"));
    return NDN_ERROR_SocketTransport_cannot_connect_to_socket;
  }

  m_udp.write(pkt, len);
  res = m_udp.endPacket();
  if (res != 1) {
    UDPTRANSPORT_DBG(F("Udp::endPacket error"));
    return NDN_ERROR_SocketTransport_error_in_send;
  }

  return NDN_ERROR_success;
}

} // namespace ndn