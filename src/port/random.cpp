#include "random.hpp"

#if defined(ARDUINO_ARCH_ESP8266)
#include <Arduino.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <esp_system.h>
#elif defined(ARDUINO_ARCH_NRF52)
#include <Arduino.h>
#include <algorithm>
#include <nrf_soc.h>
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
#elif defined(ARDUINO_ARCH_NRF52)
  while (count > 0) {
    uint8_t nAvail = 0;
    uint32_t err = sd_rand_application_bytes_available_get(&nAvail);
    if (err != NRF_SUCCESS) {
      return false;
    }
    if (nAvail == 0) {
      yield();
      continue;
    }
    size_t nRead = std::min<size_t>(count, nAvail);
    err = sd_rand_application_vector_get(output, nRead);
    if (err != NRF_SUCCESS) {
      return false;
    }
    output += nRead;
    count -= nRead;
  }
  return true;
#else
  return false;
#endif
}

} // namespace ndnph_port
} // namespace esp8266ndn
