#ifndef ESP8266NDN_SHA256_IMPL_BEARSSL_HPP
#define ESP8266NDN_SHA256_IMPL_BEARSSL_HPP

#include <bearssl/bearssl_hash.h>
#include <bearssl/bearssl_hmac.h>

namespace ndn {
namespace detail {

static void
sha256(const uint8_t* input, size_t len, uint8_t* result)
{
  br_sha256_context ctx;
  br_sha256_init(&ctx);
  br_sha256_update(&ctx, input, len);
  br_sha256_out(&ctx, result);
}

class HmacImpl
{
public:
  HmacImpl(const uint8_t* secret, size_t secretLen)
  {
    br_hmac_key_init(&m_key, &br_sha256_vtable, secret, secretLen);
  }

  void
  computeHmac(const uint8_t* input, size_t inputLen, uint8_t* sig) const
  {
    br_hmac_context ctx;
    br_hmac_init(&ctx, &m_key, 0);
    br_hmac_update(&ctx, input, inputLen);
    br_hmac_out(&ctx, sig);
  }

private:
  br_hmac_key_context m_key;
};

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_SHA256_IMPL_BEARSSL_HPP
