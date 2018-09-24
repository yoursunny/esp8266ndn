#ifndef ESP8266NDN_MBEDTLS_HELPER_HPP
#define ESP8266NDN_MBEDTLS_HELPER_HPP

#include "../../ndn-cpp/lite/util/crypto-lite.hpp"

namespace ndn {
namespace detail {

inline int
mbedRng(void*, unsigned char* dest, size_t size)
{
  CryptoLite::generateRandomBytes(dest, size);
  return 0;
}

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_MBEDTLS_HELPER_HPP
