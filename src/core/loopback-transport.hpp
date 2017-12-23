#ifndef ESP8266NDN_LOOPBACK_TRANSPORT_HPP
#define ESP8266NDN_LOOPBACK_TRANSPORT_HPP

#include "transport.hpp"

namespace ndn {

/** \brief packet size stored by LoopbackTransport
 */
#define LOOPBACKTRANSPORT_PKTSIZE 1500

/** \brief a transport that talks to another LoopbackTransport
 *
 *  This transport can store one packet. Additional received packets are lost.
 */
class LoopbackTransport : public Transport
{
public:
  LoopbackTransport();

  void
  begin(LoopbackTransport& other);

  size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t* endpointId) final;

  void
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) final;

private:
  LoopbackTransport* m_other;

  uint8_t m_pkt[LOOPBACKTRANSPORT_PKTSIZE]; ///< received packet
  size_t m_len; ///< packet size
  uint64_t m_endpointId; ///< packet endpointId
};

} // namespace ndn

#endif // ESP8266NDN_LOOPBACK_TRANSPORT_HPP
