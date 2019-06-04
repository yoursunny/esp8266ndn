#ifndef ESP8266NDN_UDP_TRANSPORT_HPP
#define ESP8266NDN_UDP_TRANSPORT_HPP

#if defined(ESP8266) || defined(ESP32)

#include "transport.hpp"

#if defined(ESP8266)
// cause WiFiUDP.h to use ESP8266WiFi instead of Arduino WiFi library
#include <ESP8266WiFi.h>
#endif
#include <WiFiUdp.h>

namespace ndn {

/** \brief a transport that communicates over UDP tunnel to a remote router
 */
class UdpTransport : public PollModeTransport
{
public:
  /** \brief interpretation of endpointId
   */
  union EndpointId {
    uint64_t endpointId;
    struct {
      uint32_t ip;
      uint16_t port;
      char _a[2];
    };
  };

  UdpTransport();

  /** \begin listen on a UDP port for packets from any remote endpoint
   *  \param localPort local port
   *  \param localIp local interface address (ignored on ESP8266)
   */
  bool
  beginListen(uint16_t localPort = 6363, IPAddress localIp = INADDR_NONE);

  /** \begin establish a UDP tunnel to a remote endpoint
   *  \param localIp local interface address
   *                 (ignored on ESP8266 for unicast; ignored on ESP32 for multicast)
   *  \param localPort listening port
   *  \param joinMulticast whether to join multicast group
   */
  bool
  beginTunnel(IPAddress remoteIp, uint16_t remotePort = 6363, uint16_t localPort = 6363);

  /** \begin join a UDP multicast group
   *  \param localIp local interface address (ignored on ESP32)
   *  \param groupPort multicast group port
   */
  bool
  beginMulticast(IPAddress localIp = INADDR_NONE, uint16_t groupPort = 56363);

  /** \begin disable the transport
   */
  void
  end();

  /** \begin receive a packet
   *  \param[out] endpointId identity of remote endpoint
   */
  size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId) final;

  /** \begin transmit a packet
   *  \param endpointId identity of remote endpoint
   *
   *  If \p endpointId is zero: sending fails in LISTEN mode, send to remote endpoint in
   *  TUNNEL mode, send to multicast group in MULTICAST mode.
   */
  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) final;

public:
  static const IPAddress MCAST_GROUP;

private:
  WiFiUDP m_udp;

  enum class Mode {
    NONE,
    LISTEN,
    TUNNEL,
    MULTICAST,
  };
  Mode m_mode;

  IPAddress m_ip;  ///< remote IP in TUNNEL mode, local IP in MULTICAST mode
  uint16_t m_port; ///< remote port in TUNNEL mode, group port in MULTICAST mode
};

} // namespace ndn

#endif // defined(ESP8266) || defined(ESP32)

#endif // ESP8266NDN_UDP_TRANSPORT_HPP
