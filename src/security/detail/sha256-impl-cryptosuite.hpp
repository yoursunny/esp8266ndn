#ifndef ESP8266NDN_SHA256_IMPL_CRYPTOSUITE_HPP
#define ESP8266NDN_SHA256_IMPL_CRYPTOSUITE_HPP

#include "../../vendor/cryptosuite-sha256.h"

namespace ndn {
namespace detail {

inline void
sha256(const uint8_t* input, size_t len, uint8_t* result)
{
  Sha256 sha;
  sha.init();
  sha.write(input, len);
  memcpy(result, sha.result(), HASH_LENGTH);
}

class HmacImpl
{
public:
  HmacImpl(const uint8_t* secret, size_t secretLen)
  {
    m_sha.initHmac(secret, secretLen);
  }

  void
  computeHmac(const uint8_t* input, size_t inputLen, uint8_t* sig) const
  {
    m_sha.reset();
    m_sha.write(input, inputLen);
    memcpy(sig, m_sha.resultHmac(), HASH_LENGTH);
  }

private:
  mutable Sha256 m_sha;
};

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_SHA256_IMPL_CRYPTOSUITE_HPP
