#ifndef ESP8266NDN_SHA256_IMPL_MBEDTLS_HPP
#define ESP8266NDN_SHA256_IMPL_MBEDTLS_HPP

#include <mbedtls/md.h>
#include <mbedtls/sha256.h>

namespace ndn {
namespace detail {

inline void
sha256(const uint8_t* input, size_t len, uint8_t* result)
{
  mbedtls_sha256_ret(input, len, result, 0);
}

class HmacImpl
{
public:
  HmacImpl(const uint8_t* secret, size_t secretLen)
  {
    mbedtls_md_init(&m_ctx);
    mbedtls_md_setup(&m_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    mbedtls_md_hmac_starts(&m_ctx, secret, secretLen);
  }

  ~HmacImpl()
  {
    mbedtls_md_free(&m_ctx);
  }

  void
  computeHmac(const uint8_t* input, size_t inputLen, uint8_t* sig) const
  {
    auto ctx = const_cast<mbedtls_md_context_t*>(&m_ctx);
    mbedtls_md_hmac_update(ctx, input, inputLen);
    mbedtls_md_hmac_finish(ctx, sig);
    mbedtls_md_hmac_reset(ctx);
  }

private:
  mbedtls_md_context_t m_ctx;
};

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_SHA256_IMPL_MBEDTLS_HPP
