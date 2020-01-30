#ifndef ESP8266NDN_PORT_RANDOM_HPP
#define ESP8266NDN_PORT_RANDOM_HPP

#include <cstdint>
#include <cstdlib>

namespace esp8266ndn {
namespace ndnph_port {

/**
 * @brief Hardware random bytes generator.
 *
 * ESP8266 and ESP32: WiFi or Bluetooth must be enabled.
 */
class RandomSource
{
public:
  RandomSource() = delete;

  static bool generate(uint8_t* output, size_t count);
};

} // namespace ndnph_port
} // namespace esp8266ndn

namespace ndnph {
namespace port {
using RandomSource = esp8266ndn::ndnph_port::RandomSource;
} // namespace port
} // namespace ndnph

#endif // ESP8266NDN_PORT_RANDOM_HPP
