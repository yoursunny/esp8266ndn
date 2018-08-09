#ifndef ESP8266NDN_UNICAST_UDP_TRANSPORT_HPP
#define ESP8266NDN_UNICAST_UDP_TRANSPORT_HPP

#include "udp-transport.hpp"

namespace ndn {

/** \brief a transport that communicates over UDP tunnel to a remote router
 *  \deprecated use UdpTransport with beginTunnel
 */
class UnicastUdpTransport : public UdpTransport
{
public:
  explicit
  UnicastUdpTransport(UDP& udp)
  {
  }

  void
  begin(IPAddress routerIp, uint16_t routerPort, uint16_t localPort)
  {
    this->beginTunnel(routerIp, routerPort, localPort);
  }
} __attribute__((deprecated));

} // namespace ndn

#endif // ESP8266NDN_UNICAST_UDP_TRANSPORT_HPP
