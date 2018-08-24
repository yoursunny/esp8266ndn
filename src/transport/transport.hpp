#ifndef ESP8266NDN_TRANSPORT_HPP
#define ESP8266NDN_TRANSPORT_HPP

#include "../ndn-cpp/c/errors.h"
#include <cinttypes>
#include <cstddef>

namespace ndn {

/** \brief a Transport sends and receives NDN packets over the network
 */
class Transport
{
public:
  /** \brief receive a packet
   *  \param buf receive buffer
   *  \param bufSize receive buffer size
   *  \param[out] endpointId identifier of the remote endpoint
   *  \return size of received packet, or zero if no packet available
   */
  virtual size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId) = 0;

  /** \brief send a packet
   *  \param pkt packet to send
   *  \param len packet size
   *  \param endpointId identifier of the remote endpoint
   */
  virtual ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) = 0;
};

} // namespace ndn

#endif // ESP8266NDN_TRANSPORT_HPP
