#ifndef ESP8266NDN_NLTRANSPORT_HPP
#define ESP8266NDN_NLTRANSPORT_HPP

#include "../transport/transport.hpp"

#include "../ndn-cpp/lite/name-lite.hpp"

#include <memory>

namespace ndn {

/** \brief An application face in NDN-Lite stack.
 *
 *  This should be used by a \c Face to form an application.
 */
class NlTransport : public Transport
{
public:
  NlTransport();

  ~NlTransport();

  /** \brief Register this face with NDN-Lite forwarder.
   */
  bool
  begin();

  /** \brief Unregister this face with NDN-Lite forwarder.
   */
  bool
  end();

  /** \brief Add a route in NDN-Lite FIB.
   *  \post Interests matching \p name are received on this transport.
   */
  ndn_Error
  addRoute(const NameLite& name);

  /** \brief Send a packet to NDN-Lite forwarder.
   */
  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) override;

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // ESP8266NDN_NLTRANSPORT_HPP
