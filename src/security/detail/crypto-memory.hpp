#ifndef ESP8266NDN_CRYPTO_MEMORY_HPP
#define ESP8266NDN_CRYPTO_MEMORY_HPP

#include <cstddef>
#include <cstdint>

namespace ndn {

inline bool
compareDigest(const uint8_t* a, const uint8_t* b, size_t len)
{
  uint8_t res = 0;
  for (size_t i = 0; i < len; ++i) {
    res |= a[i] ^ b[i];
  }
  return res == 0;
}

} // namespace ndn

#endif // ESP8266NDN_CRYPTO_MEMORY_HPP
