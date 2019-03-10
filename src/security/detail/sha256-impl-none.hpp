#ifndef ESP8266NDN_SHA256_IMPL_NONE_HPP
#define ESP8266NDN_SHA256_IMPL_NONE_HPP

#include <cstring>
#include "../../ndn-cpp/c/common.h"

namespace ndn {
namespace detail {

static void
sha256(const uint8_t* input, size_t len, uint8_t* result)
{
  memset(result, 0xDD, ndn_SHA256_DIGEST_SIZE);
}

class HmacImpl
{
public:
  HmacImpl(const uint8_t* secret, size_t secretLen)
  {
  }

  void
  computeHmac(const uint8_t* input, size_t inputLen, uint8_t* sig) const
  {
    memset(sig, 0xDD, ndn_SHA256_DIGEST_SIZE);
  }
};

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_SHA256_IMPL_NONE_HPP
