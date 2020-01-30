#ifndef ESP8266NDN_TRANSPORT_UDP_TRANSPORT_HPP
#define ESP8266NDN_TRANSPORT_UDP_TRANSPORT_HPP

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "../port/port.hpp"

#if defined(ARDUINO_ARCH_ESP8266)
// cause WiFiUdp.h to use ESP8266WiFi instead of Arduino WiFi library
#include <ESP8266WiFi.h>
#endif
#include <WiFiUdp.h>

namespace esp8266ndn {

/** @brief A transport that communicates over IPv4 UDP tunnel or multicast group. */
class UdpTransport : public virtual ndnph::Transport
{
public:
  /** @brief Interpretation of endpointId. */
  union EndpointId
  {
    uint64_t endpointId;
    struct
    {
      uint32_t ip;
      uint16_t port;
      char _a[2];
    };
  };

  /**
   * @brief Listen on a UDP port for packets from any remote endpoint.
   * @param localPort local port.
   * @param localIp local interface address (ESP32 only).
   */
  bool beginListen(uint16_t localPort = 6363, IPAddress localIp = INADDR_NONE);

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
  bool beginMulticast(IPAddress localIp = INADDR_NONE, uint16_t groupPort = 56363);

  /** @brief Disable the transport. */
  void end();

private:
  bool doIsUp() const final;

  void doLoop() final;

  bool doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId) final;

public:
  static const IPAddress MCAST_GROUP;

private:
  WiFiUDP m_udp;

  enum class Mode
  {
    NONE,
    LISTEN,
    TUNNEL,
    MULTICAST,
  };
  Mode m_mode = Mode::NONE;

  IPAddress m_ip;      ///< remote IP in TUNNEL mode, local IP in MULTICAST mode
  uint16_t m_port = 0; ///< remote port in TUNNEL mode, group port in MULTICAST mode

  ndnph::StaticRegion<1500> m_region;
};

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#endif // ESP8266NDN_TRANSPORT_UDP_TRANSPORT_HPP
