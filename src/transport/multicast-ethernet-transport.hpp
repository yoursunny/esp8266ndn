#ifndef ESP8266NDN_MULTICAST_ETHERNET_TRANSPORT_HPP
#define ESP8266NDN_MULTICAST_ETHERNET_TRANSPORT_HPP

#include "ethernet-transport.hpp"

namespace ndn {

/** \brief a transport that communicates over Ethernet multicast
 */
class MulticastEthernetTransport : public EthernetTransport
{
public:
  MulticastEthernetTransport() = default;

  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) final
  {
    return this->EthernetTransport::send(pkt, len, 0);
  }
} __attribute__((deprecated));

} // namespace ndn

#endif // ESP8266NDN_MULTICAST_ETHERNET_TRANSPORT_HPP
