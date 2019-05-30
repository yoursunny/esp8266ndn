#ifndef ESP8266NDN_PACKET_HANDLER_HPP
#define ESP8266NDN_PACKET_HANDLER_HPP

#include "packet-buffer.hpp"

namespace ndn {

class Face;

/** \brief Base class to receive callbacks from Face.
 */
class PacketHandler
{
public:
  /** \brief Construct without adding to face.
   */
  PacketHandler();

  /** \brief Construct and add handler to face.
   */
  explicit
  PacketHandler(Face& face, int8_t prio = 0);

  /** \brief Remove handler from face.
   */
  virtual
  ~PacketHandler();

protected:
  Face*
  getFace() const
  {
    return m_face;
  }

private:
  virtual bool
  processInterest(const InterestLite& interest, uint64_t endpointId);

  virtual bool
  processData(const DataLite& data, uint64_t endpointId);

  virtual bool
  processNack(const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId);

private:
  Face* m_face = nullptr;
  PacketHandler* m_next = nullptr;
  int8_t m_prio = 0;

  friend class Face;
};

} // namespace ndn

#endif // ESP8266NDN_PACKET_HANDLER_HPP
