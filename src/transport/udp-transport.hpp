#ifndef ESP8266NDN_UDP_TRANSPORT_HPP
#define ESP8266NDN_UDP_TRANSPORT_HPP

#include "transport.hpp"
#include <Udp.h>

namespace ndn {

/** \brief a transport that communicates over UDP tunnel to a remote router
 */
class UdpTransport : public Transport
{
public:
  union EndpointId {
    uint64_t endpointId;
    struct {
      uint32_t ip;
      uint16_t port;
      char _a[2];
    };
  };

  explicit
  UdpTransport(UDP& udp);

  /** \begin enable the transport
   *  \param localPort listening port
   */
  bool
  begin(uint16_t localPort);

  /** \begin disable the transport
   */
  void
  end();

  /** \begin limit the transport to talk to only one remote node
   *  \param remoteIp an IPv4 unicast address
   *  \param remotePort a UDP port number
   */
  void
  connect(IPAddress remoteIp, uint16_t remotePort);

  /** \begin receive a packet
   *
   *  If connect() has not been invoked, the transport accepts packets from any IPv4 unicast
   *  sender. Identity of sender is reflected in \p endpointId.
   *  If connect() has been invoked, the transport accepts packets from the specified sender only,
   *  and \p endpointId remains zero.
   */
  size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t* endpointId) final;

  /** \begin transmit a packet
   *  \param endpointId Identity of remote endpoint.
   *
   *  If \p endpointId is zero: if connect() has not been invoked, the packet is sent to the NDN
   *  multicast group. If connect() has been invoked, the packet is not sent.
   */
  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) final;

public:
  static const IPAddress MCAST_GROUP;

private:
  UDP& m_udp;
  IPAddress m_remoteIp;
  uint16_t m_remotePort;
};

} // namespace ndn

#endif // ESP8266NDN_UDP_TRANSPORT_HPP
