#if defined(ESP8266) || defined(ESP32)

#include "udp-transport.hpp"
#include "../core/logger.hpp"

#define LOG(...) LOGGER(UdpTransport, __VA_ARGS__)

namespace ndn {

static_assert(sizeof(UdpTransport::EndpointId) == sizeof(uint64_t), "");

const IPAddress UdpTransport::MCAST_GROUP(224, 0, 23, 170);

UdpTransport::UdpTransport()
  : m_mode(Mode::NONE)
  , m_port(0)
{
}

bool
UdpTransport::beginListen(uint16_t localPort, IPAddress localIp)
{
  end();
#if defined(ESP8266)
  LOG(F("listening on 0.0.0.0:") << _DEC(localPort));
  bool ok = m_udp.begin(localPort);
#elif defined(ESP32)
  LOG(F("listening on ") << localIp << ':' << _DEC(localPort));
  bool ok = m_udp.begin(localIp, localPort);
#endif
  if (ok) {
    m_mode = Mode::LISTEN;
  }
  return ok;
}

bool
UdpTransport::beginTunnel(IPAddress remoteIp, uint16_t remotePort, uint16_t localPort)
{
  end();
  LOG(F("connecting to ") << remoteIp << ':' << remotePort <<
                   F(" from :") << _DEC(localPort));
  bool ok = m_udp.begin(localPort);
  if (ok) {
    m_mode = Mode::TUNNEL;
    m_ip = remoteIp;
    m_port = remotePort;
  }
  return ok;
}

bool
UdpTransport::beginMulticast(IPAddress localIp, uint16_t groupPort)
{
  end();
#if defined(ESP8266)
  LOG(F("joining group ") << MCAST_GROUP << ':' << _DEC(groupPort) <<
                   F(" on ") << localIp);
  bool ok = m_udp.beginMulticast(localIp, MCAST_GROUP, groupPort);
#elif defined(ESP32)
  LOG(F("joining group ") << MCAST_GROUP << ':' << _DEC(groupPort));
  bool ok = m_udp.beginMulticast(MCAST_GROUP, groupPort);
#endif
  if (ok) {
    m_mode = Mode::MULTICAST;
    m_ip = localIp;
    m_port = groupPort;
  }
  return ok;
}

void
UdpTransport::end()
{
  m_mode = Mode::NONE;
  m_ip = INADDR_NONE;
  m_port = 0;
  m_udp.stop();
}

size_t
UdpTransport::receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId)
{
  if (m_mode == Mode::NONE) {
    return 0;
  }
  while (m_udp.parsePacket() > 0) {
    if (m_mode == Mode::TUNNEL) {
      if (m_udp.remoteIP() != m_ip || m_udp.remotePort() != m_port) {
        m_udp.flush();
        continue;
      }
      endpointId = 0;
    }
    else {
      EndpointId endpoint = {0};
      endpoint.ip = static_cast<uint32_t>(m_udp.remoteIP());
      endpoint.port = m_udp.remotePort();
      endpointId = endpoint.endpointId;
    }

    int len = m_udp.read(buf, bufSize);
    m_udp.flush();
    if (len <= 0) {
      continue;
    }
    return static_cast<size_t>(len);
  }
  return 0;
}

ndn_Error
UdpTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  int res = -1;
  if (endpointId == 0) {
    switch (m_mode) {
      case Mode::LISTEN:
        LOG(F("remote endpoint not specified"));
        return NDN_ERROR_SocketTransport_error_in_getaddrinfo;
      case Mode::TUNNEL:
        res = m_udp.beginPacket(m_ip, m_port);
        break;
      case Mode::MULTICAST:
#if defined(ESP8266)
        res = m_udp.beginPacketMulticast(MCAST_GROUP, m_port, m_ip);
#elif defined(ESP32)
        res = m_udp.beginMulticastPacket();
#endif
        break;
      case Mode::NONE:
        return NDN_ERROR_SocketTransport_socket_is_not_open;
    }
  }
  else {
    if (m_mode == Mode::NONE) {
      return NDN_ERROR_SocketTransport_socket_is_not_open;
    }
    EndpointId endpoint;
    endpoint.endpointId = endpointId;
    res = m_udp.beginPacket(IPAddress(endpoint.ip), endpoint.port);
  }

  if (res != 1) {
    LOG(F("Udp::beginPacket error"));
    return NDN_ERROR_SocketTransport_cannot_connect_to_socket;
  }

  m_udp.write(pkt, len);
  res = m_udp.endPacket();
  if (res != 1) {
    LOG(F("Udp::endPacket error"));
    return NDN_ERROR_SocketTransport_error_in_send;
  }

  return NDN_ERROR_success;
}

} // namespace ndn

#endif // defined(ESP8266) || defined(ESP32)
