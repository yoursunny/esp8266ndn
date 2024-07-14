#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)

#include "udp-transport.hpp"
#include "../core/logger.hpp"

#define LOG(...) LOGGER(UdpTransport, __VA_ARGS__)

namespace esp8266ndn {

const IPAddress UdpTransport::MulticastGroup(224, 0, 23, 170);

UdpTransport::UdpTransport(size_t mtu) {
  m_ownBuf.reset(new uint8_t[mtu]);
  m_buf = m_ownBuf.get();
  m_bufcap = mtu;
}

UdpTransport::UdpTransport(uint8_t* buffer, size_t capacity)
  : m_buf(buffer)
  , m_bufcap(capacity) {}

bool
UdpTransport::beginListen(uint16_t localPort, IPAddress localIp) {
  end();
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_RP2040)
#if LWIP_IPV6
  LOG(F("listening on [::]:") << _DEC(localPort));
#else
  LOG(F("listening on 0.0.0.0:") << _DEC(localPort));
#endif
  bool ok = m_udp.begin(localPort);
#elif defined(ARDUINO_ARCH_ESP32)
  LOG(F("listening on ") << localIp << ':' << _DEC(localPort));
  bool ok = localIp.type() == IPType::IPv4 && uint32_t(localIp) == 0
              ? m_udp.begin(localPort)
              : m_udp.begin(localIp, localPort);
#endif
  if (ok) {
    m_mode = Mode::LISTEN;
  }
  return ok;
}

bool
UdpTransport::beginTunnel(IPAddress remoteIp, uint16_t remotePort, uint16_t localPort) {
  end();
  LOG(F("connecting to ") << remoteIp << ':' << remotePort << F(" from :") << _DEC(localPort));
  bool ok =
#if defined(ARDUINO_ARCH_ESP32) && LWIP_IPV6
    remoteIp.type() == IPType::IPv6 ? m_udp.begin(IN6ADDR_ANY, localPort) :
#endif
                                    m_udp.begin(localPort);
  if (ok) {
    m_mode = Mode::TUNNEL;
    m_ip = remoteIp;
    m_port = remotePort;
  }
  return ok;
}

bool
UdpTransport::beginMulticast(IPAddress localIp, uint16_t groupPort) {
  end();
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_RP2040)
  LOG(F("joining group ") << MulticastGroup << ':' << _DEC(groupPort) << F(" on ") << localIp);
  bool ok = m_udp.beginMulticast(localIp, MulticastGroup, groupPort);
#elif defined(ARDUINO_ARCH_ESP32)
  LOG(F("joining group ") << MulticastGroup << ':' << _DEC(groupPort));
  bool ok = m_udp.beginMulticast(MulticastGroup, groupPort);
#endif
  if (ok) {
    m_mode = Mode::MULTICAST;
    m_ip = localIp;
    m_port = groupPort;
  }
  return ok;
}

void
UdpTransport::end() {
  m_mode = Mode::NONE;
  m_ip = INADDR_NONE;
  m_port = 0;
  m_udp.stop();
}

bool
UdpTransport::doIsUp() const {
  return m_mode != Mode::NONE;
}

void
UdpTransport::doLoop() {
  if (m_mode == Mode::NONE) {
    return;
  }
  for (int pktLen = m_udp.parsePacket(); pktLen > 0; pktLen = m_udp.parsePacket()) {
    uint64_t endpointId = 0;
    if (m_mode == Mode::TUNNEL) {
      if (m_udp.remoteIP() != m_ip || m_udp.remotePort() != m_port) {
        m_udp.flush();
        continue;
      }
    } else {
      IPAddress ip = m_udp.remoteIP();
      uint16_t port = m_udp.remotePort();
#if LWIP_IPV6
#if defined(ARDUINO_ARCH_ESP8266)
      if (ip.isV6()) {
        endpointId = m_endpoints.encode(reinterpret_cast<const uint8_t*>(ip.raw6()), 16, port);
      } else
#elif defined(ARDUINO_ARCH_ESP32)
      if (ip.type() == IPType::IPv6) {
        uint8_t addr[16];
        for (int i = 0; i < 16; ++i) {
          addr[i] = ip[i];
        }
        endpointId = m_endpoints.encode(addr, 16, port);
      } else
#endif
#endif
      {
        uint32_t ip4 = ip;
        endpointId = m_endpoints.encode(reinterpret_cast<const uint8_t*>(&ip4), 4, port);
      }
    }

    if (static_cast<size_t>(pktLen) > m_bufcap) {
      LOG(F("packet longer than buffer capacity pktLen=") << pktLen);
      continue;
    }

    int len = m_udp.read(m_buf, pktLen);
    m_udp.flush();
    if (len <= 0) {
      continue;
    }
    invokeRxCallback(m_buf, pktLen, endpointId);
  }
}

bool
UdpTransport::doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId) {
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
        ok = m_udp.beginPacketMulticast(MulticastGroup, m_port, m_ip);
#elif defined(ARDUINO_ARCH_ESP32)
        ok = m_udp.beginMulticastPacket();
#endif
        break;
      case Mode::NONE:
        return false;
    }
  } else if (m_mode == Mode::NONE) {
    return false;
  } else {
    uint8_t addr[16];
    uint16_t port = 0;
    size_t addrLen = m_endpoints.decode(endpointId, addr, &port);
    IPAddress ip;
    switch (addrLen) {
      case 4:
        ip = addr;
        break;
#if LWIP_IPV6
      case 16:
#if defined(ARDUINO_ARCH_ESP8266)
        std::copy_n(addr, addrLen, reinterpret_cast<uint8_t*>(ip.raw6()));
#elif defined(ARDUINO_ARCH_ESP32)
        ip = IPAddress(IPType::IPv6, addr);
#endif
        break;
#endif
      default:
        return false;
    }
    ok = m_udp.beginPacket(ip, port);
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
