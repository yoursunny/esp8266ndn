#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "udp-transport.hpp"
#include "../core/logger.hpp"

#define LOG(...) LOGGER(UdpTransport, __VA_ARGS__)

namespace esp8266ndn {

namespace {

union EndpointId
{
  uint64_t id;
  struct
  {
    uint32_t v4;
    uint16_t port;
    uint16_t zero4;
  };
  struct
  {
    // [____:____:____:____:AABB:CCD_:EEFF:GGHH]:____
    uint8_t v6a;
    uint8_t v6b;
    uint8_t v6c;
    uint8_t v6e;
    uint8_t v6f;
    uint8_t v6g;
    uint8_t v6h;
    uint8_t v6d : 5;
    uint8_t v6ref : 2;
    bool isV6 : 1;
  };
};

static_assert(sizeof(EndpointId) == sizeof(uint64_t), "");

} // anonymous namespace

const IPAddress UdpTransport::MulticastGroup(224, 0, 23, 170);

UdpTransport::UdpTransport(size_t mtu)
{
  m_ownBuf.reset(new uint8_t[mtu]);
  m_buf = m_ownBuf.get();
  m_bufcap = mtu;
}

UdpTransport::UdpTransport(uint8_t* buffer, size_t capacity)
  : m_buf(buffer)
  , m_bufcap(capacity)
{}

bool
UdpTransport::beginListen(uint16_t localPort, IPAddress localIp)
{
  end();
#if defined(ARDUINO_ARCH_ESP8266)
#if LWIP_IPV6
  LOG(F("listening on [::]:") << _DEC(localPort));
#else
  LOG(F("listening on 0.0.0.0:") << _DEC(localPort));
#endif
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
UdpTransport::end()
{
  m_mode = Mode::NONE;
  m_ip = INADDR_NONE;
  m_port = 0;
  m_udp.stop();
}

uint64_t
UdpTransport::toEndpointId(IPAddress ip, uint16_t port)
{
  EndpointId ep;
#if defined(ARDUINO_ARCH_ESP8266) && LWIP_IPV6
  if (ip.isV6()) {
    auto raw6 = reinterpret_cast<const uint8_t*>(ip.raw6());
    ep.v6a = raw6[8];
    ep.v6b = raw6[9];
    ep.v6c = raw6[10];
    ep.v6d = raw6[11] >> 3;
    ep.v6e = raw6[12];
    ep.v6f = raw6[13];
    ep.v6g = raw6[14];
    ep.v6h = raw6[15];

    // [CCDD:EEFF:GGHH:IIJJ:____:___K:____:____]:AABB
    std::array<uint8_t, 11> v6r{};
    *reinterpret_cast<uint16_t*>(&v6r[0]) = m_udp.remotePort();
    std::copy_n(raw6, 8, &v6r[2]);
    v6r[10] = raw6[11] & 0x07;

    auto found = std::find(m_v6Intern.begin(), m_v6Intern.end(), v6r);
    if (found != m_v6Intern.end()) {
      ep.v6ref = std::distance(m_v6Intern.begin(), found);
    } else {
      m_v6Intern[m_v6InternPos] = v6r;
      ep.v6ref = m_v6InternPos;
      if (++m_v6InternPos == m_v6Intern.size()) {
        m_v6InternPos = 0;
      }
    }
    ep.isV6 = true;
  } else
#endif // LWIP_IPV6
  {
    ep.v4 = static_cast<uint32_t>(ip);
    ep.port = port;
  }
  return ep.id;
}

std::tuple<IPAddress, uint16_t>
UdpTransport::fromEndpointId(uint64_t id)
{
  EndpointId ep = { .id = id };
  IPAddress ip;
  uint16_t port = 0;
#if defined(ARDUINO_ARCH_ESP8266) && LWIP_IPV6
  if (ep.isV6) {
    const auto& v6r = m_v6Intern[ep.v6ref];
    memcpy(&port, &v6r[0], 2);

    auto raw6 = reinterpret_cast<uint8_t*>(ip.raw6());
    std::copy_n(&v6r[2], 8, raw6);
    raw6[8] = ep.v6a;
    raw6[9] = ep.v6b;
    raw6[10] = ep.v6c;
    raw6[11] = (ep.v6d << 3) | v6r[10];
    raw6[12] = ep.v6e;
    raw6[13] = ep.v6f;
    raw6[14] = ep.v6g;
    raw6[15] = ep.v6h;
  } else
#endif // LWIP_IPV6
  {
    ip = ep.v4;
    port = ep.port;
  }
  return std::make_tuple(ip, port);
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
    uint64_t endpointId = 0;
    if (m_mode == Mode::TUNNEL) {
      if (m_udp.remoteIP() != m_ip || m_udp.remotePort() != m_port) {
        m_udp.flush();
        continue;
      }
    } else {
      endpointId = toEndpointId(m_udp.remoteIP(), m_udp.remotePort());
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
    IPAddress ip;
    uint16_t port = 0;
    std::tie(ip, port) = fromEndpointId(endpointId);
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
