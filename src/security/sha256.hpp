#ifndef ESP8266NDN_SHA256_HPP
#define ESP8266NDN_SHA256_HPP

#include "../ndn-cpp/cryptosuite/sha256.h"

namespace ndn {

inline const uint8_t*
computeSha256Hash(const uint8_t* input, size_t len)
{
  Sha256.init();
  for (size_t i = 0; i < len; ++i) {
    Sha256.write(input[i]);
  }
  return Sha256.result();
}

} // namespace ndn

#endif // ESP8266NDN_SHA256_HPP
