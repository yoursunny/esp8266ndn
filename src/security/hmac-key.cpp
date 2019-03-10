#include "hmac-key.hpp"
#include "detail/crypto-memory.hpp"
#include "detail/sha256-impl.hpp"
#include "../ndn-cpp/lite/util/crypto-lite.hpp"

namespace ndn {

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
