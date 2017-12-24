#include "hmac-key.hpp"
#include "../ndn-cpp/lite/util/crypto-lite.hpp"
#include <cstring>

namespace ndn {

HmacKey::HmacKey(const uint8_t* secret, size_t secretLen)
  : m_secret(secret)
  , m_secretLen(secretLen)
{
  CryptoLite::digestSha256(m_secret, m_secretLen, m_keyDigest);
}

int
HmacKey::sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const
{
  CryptoLite::computeHmacWithSha256(m_secret, m_secretLen, input, inputLen, sig);
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
  CryptoLite::computeHmacWithSha256(m_secret, m_secretLen, input, inputLen, newSig);
  return std::memcmp(sig, newSig, ndn_SHA256_DIGEST_SIZE) == 0;
}

} // namespace ndn
