#include "hmac-key.hpp"
#include "detail/crypto-memory.hpp"
#include "../ndn-cpp/lite/util/crypto-lite.hpp"

#if defined(ESP8266)
#include <bearssl/bearssl_hash.h>
#include <bearssl/bearssl_hmac.h>
#elif defined(ESP32)
#include <mbedtls/md.h>
#include <mbedtls/sha256.h>
#endif

namespace ndn {

class HmacKey::Impl
{
public:
  Impl(const uint8_t* secret, size_t secretLen);

  ~Impl();

  void
  computeHmac(const uint8_t* input, size_t inputLen, uint8_t* sig) const;

private:
#if defined(ESP8266)
  br_hmac_key_context m_key;
#elif defined(ESP32)
  mbedtls_md_context_t m_ctx;
#endif
};

#if defined(ESP8266)

HmacKey::Impl::Impl(const uint8_t* secret, size_t secretLen)
{
  br_hmac_key_init(&m_key, &br_sha256_vtable, secret, secretLen);
}

HmacKey::Impl::~Impl() = default;

void
HmacKey::Impl::computeHmac(const uint8_t* input, size_t inputLen, uint8_t* sig) const
{
  br_hmac_context ctx;
  br_hmac_init(&ctx, &m_key, 0);
  br_hmac_update(&ctx, input, inputLen);
  br_hmac_out(&ctx, sig);
}

#elif defined(ESP32)

HmacKey::Impl::Impl(const uint8_t* secret, size_t secretLen)
{
  mbedtls_md_init(&m_ctx);
  mbedtls_md_setup(&m_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
  mbedtls_md_hmac_starts(&m_ctx, secret, secretLen);
}

HmacKey::Impl::~Impl()
{
  mbedtls_md_free(&m_ctx);
}

void
HmacKey::Impl::computeHmac(const uint8_t* input, size_t inputLen, uint8_t* sig) const
{
  auto ctx = const_cast<mbedtls_md_context_t*>(&m_ctx);
  mbedtls_md_hmac_update(ctx, input, inputLen);
  mbedtls_md_hmac_finish(ctx, sig);
  mbedtls_md_hmac_reset(ctx);
}

#endif

HmacKey::HmacKey(const uint8_t* secret, size_t secretLen)
{
  m_impl.reset(new Impl(secret, secretLen));
  CryptoLite::digestSha256(secret, secretLen, m_keyDigest);
}

HmacKey::~HmacKey() = default;

int
HmacKey::sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const
{
  m_impl->computeHmac(input, inputLen, sig);
  return ndn_SHA256_DIGEST_SIZE;
}

ndn_Error
HmacKey::setSignatureInfo(SignatureLite& signature) const
{
  signature.setType(ndn_SignatureType_HmacWithSha256Signature);
  KeyLocatorLite& kl = signature.getKeyLocator();
  kl.setType(ndn_KeyLocatorType_KEY_LOCATOR_DIGEST);
  kl.setKeyData(BlobLite(m_keyDigest, ndn_SHA256_DIGEST_SIZE));
  return NDN_ERROR_success;
}

bool
HmacKey::verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const
{
  if (sigLen != ndn_SHA256_DIGEST_SIZE) {
    return false;
  }

  uint8_t newSig[ndn_SHA256_DIGEST_SIZE];
  m_impl->computeHmac(input, inputLen, newSig);
  return compareDigest(sig, newSig, ndn_SHA256_DIGEST_SIZE);
}

} // namespace ndn
