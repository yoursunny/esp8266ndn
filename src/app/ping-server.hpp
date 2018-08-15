#ifndef ESP8266NDN_PING_SERVER_HPP
#define ESP8266NDN_PING_SERVER_HPP

#include "../core/face.hpp"

namespace ndn {

/** \brief max NameComponent count when preparing Name of response Data
 */
#define NDNPINGSERVER_NAMECOMPS_MAX 24
/** \brief payload buffer size, in octets
 */
#define NDNPINGSERVER_PAYLOAD_MAX 256

/** \brief NDN reachability test tool server side
 */
class PingServer
{
public:
  /** \brief constructor
   *  \param face a face to communication with router
   *  \param prefix name prefix served by this PingServer; should end with 'ping'
   */
  PingServer(Face& face, const NameLite& prefix);

  ~PingServer();

  /** \brief let responses go to endpointId zero instead of incoming endpointId
   */
  void
  enableEndpointIdZero()
  {
    m_wantEndpointIdZero = true;
  }

  /** \deprecated Calling processNack is no longer necessary.
   */
  bool
  processInterest(const InterestLite& interest, uint64_t endpointId = 0) __attribute__((deprecated))
  {
    return false;
  }

  /** \brief a probe handler
   *  \param arg cbarg passed to \p onProbe
   *  \param interest the incoming Interest
   *  \param[out] payload payload of response Data
   *  \param[out] payloadSize size of payload; do not exceed NDNPINGSERVER_PAYLOAD_MAX
   */
  typedef void (*ProbeCallback)(void* arg, const InterestLite& interest, uint8_t* payload, size_t* payloadSize);

  /** \brief set probe handler
   *
   *  Only one handler is allowed. This overwrites any previous handler setting.
   */
  void
  onProbe(ProbeCallback cb, void* cbarg)
  {
    m_probeCb = cb;
    m_probeCbArg = cbarg;
  }

private:
  bool
  doProcessInterest(const InterestLite& interest, uint64_t endpointId);

  class Handler;

  friend class Handler;

private:
  Face& m_face;
  const NameLite& m_prefix;
  Handler* m_handler;
  ProbeCallback m_probeCb;
  void* m_probeCbArg;
  bool m_wantEndpointIdZero;
};

} // namespace ndn

#endif // ESP8266NDN_PING_SERVER_HPP