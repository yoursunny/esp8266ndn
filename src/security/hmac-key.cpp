#include "hmac-key.hpp"
#include <cstring>

namespace ndn {

HmacKey::HmacKey(const uint8_t* secret, size_t secretLen)
  : m_secret(secret)
  , m_secretLen(secretLen)
{
  ndn_digestSha256(m_secret, m_secretLen, m_keyDigest);
}

int
HmacKey::sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const
{
  ndn_computeHmacWithSha256(m_secret, m_secretLen, input, inputLen, sig);
  return ndn_SHA256_DIGEST_SIZE;
}

bool
HmacKey::verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const
{
  if (sigLen != ndn_SHA256_DIGEST_SIZE) {
    return false;
  }

  uint8_t newSig[ndn_SHA256_DIGEST_SIZE];
  ndn_computeHmacWithSha256(m_secret, m_secretLen, input, inputLen, newSig);
  return std::memcmp(sig, newSig, ndn_SHA256_DIGEST_SIZE) == 0;
}

} // namespace ndn
