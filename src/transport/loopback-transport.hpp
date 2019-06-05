#ifndef ESP8266NDN_LOOPBACK_TRANSPORT_HPP
#define ESP8266NDN_LOOPBACK_TRANSPORT_HPP

#include "transport.hpp"

namespace ndn {

/** \brief packet size stored by LoopbackTransport
 */
#define LOOPBACKTRANSPORT_PKTSIZE 1500

/** \brief a transport that talks to another LoopbackTransport
 */
class LoopbackTransport : public Transport
{
public:
  void
  begin(LoopbackTransport& other);

  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) final;

private:
  LoopbackTransport* m_other = nullptr;
};

} // namespace ndn

#endif // ESP8266NDN_LOOPBACK_TRANSPORT_HPP
