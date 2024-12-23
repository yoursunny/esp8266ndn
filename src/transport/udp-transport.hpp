#ifndef ESP8266NDN_TRANSPORT_UDP_TRANSPORT_HPP
#define ESP8266NDN_TRANSPORT_UDP_TRANSPORT_HPP

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)

#include "../port/port.hpp"

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_RP2040)
#include <WiFiUdp.h>
#define ESP8266NDN_NetworkUDP WiFiUDP
#elif defined(ARDUINO_ARCH_ESP32)
#include <NetworkUdp.h>
#define ESP8266NDN_NetworkUDP NetworkUDP
#endif

namespace esp8266ndn {

/** @brief A transport that communicates over UDP tunnel or multicast group. */
class UdpTransport : public virtual ndnph::Transport {
public:
  /**
   * @brief Construct using internal buffer.
   * @param mtu maximum packet length.
   *            Default is default Ethernet MTU minus IP and UDP headers.
   */
  explicit UdpTransport(size_t mtu = DefaultMtu);

  /**
   * @brief Construct using external buffer.
   * @param buffer buffer pointer. This must remain valid until transport is destructed.
   * @param capacity buffer capacity.
   */
  explicit UdpTransport(uint8_t* buffer, size_t capacity);

  /**
   * @brief Construct using external buffer.
   * @param buffer buffer array. This must remain valid until transport is destructed.
   */
  template<size_t capacity>
  explicit UdpTransport(std::array<uint8_t, capacity>& buffer)
    : UdpTransport(buffer.data(), buffer.size()) {}

  /**
   * @brief Listen on a UDP port for packets from any remote endpoint.
   * @param localPort local port.
   * @param localIp local interface address (ESP32 only).
   *
   * Up to four simultaneous IPv6 EndpointIds can be tracked.
   */
  bool beginListen(uint16_t localPort = 6363, IPAddress localIp = IPAddress());

  /**
   * @brief Establish a UDP tunnel to a remote endpoint.
   * @param remoteIp remote host address.
   * @param remotePort remote port.
   * @param localPort local port.
   */
  bool beginTunnel(IPAddress remoteIp, uint16_t remotePort = 6363, uint16_t localPort = 6363);

  /**
   * @brief Join a UDP multicast group.
   * @param localIp local interface address (ESP8266 only).
   * @param groupPort group port.
   */
  bool beginMulticast(IPAddress localIp = IPAddress(), uint16_t groupPort = 56363);

  /** @brief Disable the transport. */
  void end();

private:
  bool doIsUp() const final;

  void doLoop() final;

  bool doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId) final;

public:
  enum {
    /** @brief Default MTU for UDP is Ethernet MTU minus IPv4 and UDP headers. */
    DefaultMtu = 1500 - 20 - 8,
  };

  /** @brief NDN multicast group "224.0.23.170". */
  static const IPAddress MulticastGroup;

private:
  enum class Mode : uint8_t {
    NONE,
    LISTEN,
    TUNNEL,
    MULTICAST,
  };

  uint8_t* m_buf = nullptr;
  size_t m_bufcap = 0;
  std::unique_ptr<uint8_t[]> m_ownBuf;

  ESP8266NDN_NetworkUDP m_udp;
#if LWIP_IPV6
  using EndpointIdHelper = ndnph::port_transport_socket::Ipv6EndpointIdHelper<4>;
#else
  using EndpointIdHelper = ndnph::port_transport_socket::Ipv6EndpointIdHelper<1>;
#endif
  EndpointIdHelper m_endpoints;
  IPAddress m_ip;      ///< remote IP in TUNNEL mode, local IP in MULTICAST mode
  uint16_t m_port = 0; ///< remote port in TUNNEL mode, group port in MULTICAST mode
  Mode m_mode = Mode::NONE;
};

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#endif // ESP8266NDN_TRANSPORT_UDP_TRANSPORT_HPP
