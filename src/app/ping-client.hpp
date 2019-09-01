#ifndef ESP8266NDN_PING_CLIENT_HPP
#define ESP8266NDN_PING_CLIENT_HPP

#include "../core/face.hpp"

namespace ndn {

/** \brief NDN reachability test tool client side
 */
class PingClient : public PacketHandler
{
public:
  /** \brief Randomizable probe interval.
   */
  class Interval
  {
  public:
    /** \brief Set the probe interval within [center-variation, center+variation].
     */
    Interval(int center, int variation = 0);

    /** \brief Get a random interval.
     */
    int
    operator()() const;

public:
    const int min;
    const int max;
  };

  /** \brief constructor
   *  \param face a face to communication with router
   *  \param interest a prepared Interest for probe packets;
   *         typically its name should end with 'ping', and it should have MustBeFresh selector;
   *         the name may contain an initial sequence number,
   *         otherwise it must have space to add a sequence number
   *  \param pingInterval interval between probes, in millis
   *  \param pingTimeout probe timeout, in millis; default is InterestLifetime;
   *         must be less than \p pingInterval
   */
  PingClient(Face& face, InterestLite& interest, Interval pingInterval, int pingTimeout = -1);

  /** \brief loop the client
   */
  void
  loop();

  /** \brief send a probe now
   *  \note Response or timeout for previous probe will be ignored.
   */
  void
  probe();

  enum class Event {
    NONE,
    PROBE,
    RESPONSE,
    TIMEOUT,
    NACK,
  };

  typedef void (*EventCallback)(void* arg, Event evt, uint64_t seq);

  /** \brief set event handler
   *
   *  Only one handler is allowed. This overwrites any previous handler setting.
   */
  void
  onEvent(EventCallback cb, void* cbarg)
  {
    m_evtCb = cb;
    m_evtCbArg = cbarg;
  }

private:
  uint32_t
  getLastSeq() const;

  bool
  processData(const DataLite& data, uint64_t endpointId) override;

  bool
  processNack(const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId) override;

private:
  InterestLite& m_interest;
  uint8_t m_seqBuf[9]; ///< buffer for sequence number component
  const Interval m_pingInterval;
  const int m_pingTimeout;
  unsigned long m_lastProbe; ///< timestamp of last probe
  bool m_isPending; ///< whether lastProbe is waiting for either response or timeout
  unsigned long m_nextInterval; ///< interval between last and next probe
  EventCallback m_evtCb;
  void* m_evtCbArg;
};

} // namespace ndn

#endif // ESP8266NDN_PING_CLIENT_HPP