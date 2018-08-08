#ifndef ESP8266NDN_UNICAST_UDP_TRANSPORT_HPP
#define ESP8266NDN_UNICAST_UDP_TRANSPORT_HPP

#include "udp-transport.hpp"

namespace ndn {

/** \brief a transport that communicates over UDP tunnel to a remote router
 */
class UnicastUdpTransport : public UdpTransport
{
public:
  using UdpTransport::UdpTransport;

  void
  begin(IPAddress routerIp, uint16_t routerPort, uint16_t localPort)
  {
    this->UdpTransport::begin(localPort);
    this->connect(routerIp, routerPort);
  }
};

} // namespace ndn

#endif // ESP8266NDN_UNICAST_UDP_TRANSPORT_HPP
