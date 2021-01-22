#ifndef ESP8266NDN_UNIX_TIME_HPP
#define ESP8266NDN_UNIX_TIME_HPP

#include "../port/port.hpp"

namespace esp8266ndn {

/**
 * @brief Retrieve and maintain UnixTime clock.
 *
 * This requires a UnixTime service running in the local network.
 * @sa https://github.com/yoursunny/ndn6-tools/blob/main/unix-time-service.md
 */
class UnixTimeClass : public ndnph::PacketHandler
{
public:
  /**
   * @brief Disable integration with system clock.
   *
   * System clock integration is supported on ESP32 only, and enabled by default.
   *
   * When enabled, this module updates the system clock when a valid timestamp is received.
   * The current Unix time is then available via @c gettimeofday() and other system functions.
   * This integration cannot be used together with other time synchronization mechanisms such as
   * lwip SNTP client.
   *
   * To disable system clock integration, invoke this function before @c begin() . Then, Unix
   * timestamp is only available via UnixTime.now() function.
   */
  void disableIntegration();

  /**
   * @brief Enable UnixTime requests.
   * @param face a face to communication with server.
   * @param interval how often to refresh time (millis), minimum 5000ms.
   */
  bool begin(ndnph::Face& face, int interval = 60000);

  /** @brief Return whether UnixTime clock is available. */
  bool isAvailable() const;

  /** @brief Retrieve current UnixTime timestamp (micros). */
  uint64_t now() const;

  /** @brief Convert timestamp (micros) to RFC3399 date-time string. */
  static String toRfc3399DateTime(uint64_t timestamp);

private:
  void loop() final;

  bool processData(ndnph::Data data) final;

private:
  /// special values for m_timeOffset
  enum
  {
    TIMEOFFSET_INTEG = 0,         ///< UnixTime in sysclock
    TIMEOFFSET_INTEG_UNAVAIL = 1, ///< UnixTime unavailable, integration enabled
    TIMEOFFSET_UNAVAIL = 2,       ///< UnixTime unavailable, integration disabled
  };

  int m_interval = -1;
  unsigned long m_lastRequest = 0;
  unsigned long m_nextRequest = 0;
  uint64_t m_timeOffset = TIMEOFFSET_INTEG_UNAVAIL;
  ndnph::Interest m_interest;
};

extern UnixTimeClass UnixTime;

} // namespace esp8266ndn

#endif // ESP8266NDN_UNIX_TIME_HPP
