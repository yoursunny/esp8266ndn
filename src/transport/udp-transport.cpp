#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "udp-transport.hpp"
#include "../core/logger.hpp"

#define LOG(...) LOGGER(UdpTransport, __VA_ARGS__)

namespace esp8266ndn {

static_assert(sizeof(UdpTransport::EndpointId) == sizeof(uint64_t), "");

const IPAddress UdpTransport::MCAST_GROUP(224, 0, 23, 170);

bool
UdpTransport::beginListen(uint16_t localPort, IPAddress localIp)
{
  end();
#if defined(ARDUINO_ARCH_ESP8266)
  LOG(F("listening on 0.0.0.0:") << _DEC(localPort));
  bool ok = m_udp.begin(localPort);
#elif defined(ARDUINO_ARCH_ESP32)
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
  LOG(F("connecting to ") << remoteIp << ':' << remotePort << F(" from :") << _DEC(localPort));
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
#if defined(ARDUINO_ARCH_ESP8266)
  LOG(F("joining group ") << MCAST_GROUP << ':' << _DEC(groupPort) << F(" on ") << localIp);
  bool ok = m_udp.beginMulticast(localIp, MCAST_GROUP, groupPort);
#elif defined(ARDUINO_ARCH_ESP32)
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

bool
UdpTransport::doIsUp() const
{
  return m_mode != Mode::NONE;
}

void
UdpTransport::doLoop()
{
  if (m_mode == Mode::NONE) {
    return;
  }
  for (int pktLen = m_udp.parsePacket(); pktLen > 0; pktLen = m_udp.parsePacket()) {
    EndpointId endpoint = {};
    if (m_mode == Mode::TUNNEL) {
      if (m_udp.remoteIP() != m_ip || m_udp.remotePort() != m_port) {
        m_udp.flush();
        continue;
      }
    } else {
      endpoint.ip = static_cast<uint32_t>(m_udp.remoteIP());
      endpoint.port = m_udp.remotePort();
    }

    m_region.reset();
    uint8_t* buf = m_region.alloc(pktLen);
    if (buf == nullptr) {
      LOG(F("alloc failure for pktLen=") << pktLen);
      continue;
    }

    int len = m_udp.read(buf, pktLen);
    m_udp.flush();
    if (len <= 0) {
      continue;
    }
    invokeRxCallback(m_region, buf, pktLen, endpoint.endpointId);
  }
}

bool
UdpTransport::doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId)
{
  bool ok = false;
  if (endpointId == 0) {
    switch (m_mode) {
      case Mode::LISTEN:
        LOG(F("remote endpoint not specified"));
        return false;
      case Mode::TUNNEL:
        ok = m_udp.beginPacket(m_ip, m_port);
        break;
      case Mode::MULTICAST:
#if defined(ARDUINO_ARCH_ESP8266)
        ok = m_udp.beginPacketMulticast(MCAST_GROUP, m_port, m_ip);
#elif defined(ARDUINO_ARCH_ESP32)
        ok = m_udp.beginMulticastPacket();
#endif
        break;
      case Mode::NONE:
        return false;
    }
  } else {
    if (m_mode == Mode::NONE) {
      return false;
    }
    EndpointId endpoint = { .endpointId = endpointId };
    ok = m_udp.beginPacket(IPAddress(endpoint.ip), endpoint.port);
  }

  if (!ok) {
    LOG(F("Udp::beginPacket error"));
    return false;
  }

  m_udp.write(pkt, pktLen);
  if (!m_udp.endPacket()) {
    LOG(F("Udp::endPacket error"));
    return false;
  }

  return true;
}

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
