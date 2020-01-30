#include "random.hpp"

#if defined(ARDUINO_ARCH_ESP8266)
#include <Arduino.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <esp_system.h>
#endif

namespace esp8266ndn {
namespace ndnph_port {

bool
RandomSource::generate(uint8_t* output, size_t count)
{
#if defined(ARDUINO_ARCH_ESP8266)
  for (size_t i = 0; i < count; ++i) {
    output[i] = ::secureRandom(0x100);
  }
  return true;
#elif defined(ARDUINO_ARCH_ESP32)
  ::esp_fill_random(output, count);
  return true;
#else
  return false;
#endif
}

} // namespace ndnph_port
} // namespace esp8266ndn
