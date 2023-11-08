#ifndef ESP8266NDN_UNIX_TIME_HPP
#define ESP8266NDN_UNIX_TIME_HPP

#include "../port/port.hpp"

namespace esp8266ndn {

/**
 * @brief Retrieve and maintain UnixTime clock.
 *
 * This module requires a UnixTime service running in the local network.
 * @sa https://github.com/yoursunny/ndn6-tools/blob/main/unix-time-service.md

 * This module periodically retrieves the current Unix timestamp from the service, and then updates
 * the system clock via @c ndnph::port::UnixTime::set function. The current Unix time is then
 * available via @c ndnph::port::UnixTime::now function as well as @c gettimeofday() and other
 * system functions.
 *
 * This module cannot be used together with other time synchronization mechanisms such as
 * lwip SNTP client.
 */
class UnixTime : public ndnph::PacketHandler {
public:
  explicit UnixTime(ndnph::Face& face);

  /**
   * @brief Enable UnixTime requests.
   * @param interval how often to refresh time (millis), minimum 5000ms.
   */
  void begin(int interval = 60000);

private:
  void loop() final;

  bool processData(ndnph::Data data) final;

private:
  OutgoingPendingInterest m_pending;
  int m_interval = 0;
  ndnph::port::Clock::Time m_lastRequest;
  ndnph::port::Clock::Time m_nextRequest;
};

} // namespace esp8266ndn

#endif // ESP8266NDN_UNIX_TIME_HPP
