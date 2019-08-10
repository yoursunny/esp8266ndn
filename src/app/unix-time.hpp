#ifndef ESP8266NDN_UNIX_TIME_HPP
#define ESP8266NDN_UNIX_TIME_HPP

#include "../core/face.hpp"
#include "../core/with-components-buffer.hpp"

namespace ndn {

/** \brief Retrieve and maintain UnixTime clock.
 *
 *  This requires a UnixTime service running in the local network.
 *  \sa https://github.com/yoursunny/ndn6-tools/blob/master/unix-time-service.md
 */
class UnixTimeClass : public PacketHandler
{
public:
  UnixTimeClass();

  /** \brief Disable integration with system clock.
   *
   *  System clock integration is supported on ESP32 only, and enabled by
   *  default.
   *
   *  When enabled, this module updates the system clock when a valid timestamp
   *  is received. The current Unix time is then available via gettimeofday()
   *  and other system functions. This integration cannot be used together with
   *  other time synchronization mechanisms such as lwip SNTP client.
   *
   *  To disable system clock integration, invoke this function before begin().
   *  Then, Unix timestamp is only available via UnixTime.now() function.
   */
  void
  disableIntegration();

  /** \brief Enable UnixTime requests.
   *  \param face a face to communication with server
   *  \param interval how often to refresh time (millis)
   */
  void
  begin(Face& face, unsigned long interval = 60000);

  void
  loop();

  /** \brief Return whether UnixTime clock is available.
   */
  bool
  isAvailable() const;

  /** \brief Retrieve current UnixTime timestamp (micros).
   */
  uint64_t
  now() const;

  /** \brief Convert timestamp (micros) to RFC3399 date-time string.
   */
  static String
  toRfc3399DateTime(uint64_t timestamp);

private:
  bool
  processData(const DataLite& data, uint64_t endpointId) override;

private:
  unsigned long m_interval;
  unsigned long m_lastRequest;
  unsigned long m_nextRequest;
  uint64_t m_timeOffset;
  InterestWCB<2, 0> m_interest;
};

extern UnixTimeClass UnixTime;

} // namespace ndn

#endif // ESP8266NDN_UNIX_TIME_HPP
