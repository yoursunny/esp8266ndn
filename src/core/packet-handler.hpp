#ifndef ESP8266NDN_PACKET_HANDLER_HPP
#define ESP8266NDN_PACKET_HANDLER_HPP

#include "packet-buffer.hpp"

namespace ndn {

/** \brief Base class to receive callbacks from Face.
 */
class PacketHandler
{
private:
  virtual bool
  processInterest(const InterestLite& interest, uint64_t endpointId);

  virtual bool
  processData(const DataLite& data, uint64_t endpointId);

  virtual bool
  processNack(const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId);

private:
  PacketHandler* m_next = nullptr;

  friend class Face;
};

/** \brief legacy Interest handler
 */
typedef void (*InterestCallback)(void* arg, const InterestLite& interest, uint64_t endpointId);
/** \brief legacy Data handler
 */
typedef void (*DataCallback)(void* arg, const DataLite& data, uint64_t endpointId);
/** \brief legacy Nack handler
 */
typedef void (*NackCallback)(void* arg, const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId);

} // namespace ndn

#endif // ESP8266NDN_PACKET_HANDLER_HPP
