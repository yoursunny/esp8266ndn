#include "digest-key.hpp"
#include "../ndn-cpp/lite/util/crypto-lite.hpp"
#include <cstring>

namespace ndn {

int
DigestKey::sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const
{
  CryptoLite::digestSha256(input, inputLen, sig);
  return ndn_SHA256_DIGEST_SIZE;
}

ndn_Error
DigestKey::setSignatureInfo(SignatureLite& signature) const
{
  signature.setType(ndn_SignatureType_DigestSha256Signature);
  return NDN_ERROR_success;
}

bool
DigestKey::verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const
{
  if (sigLen != ndn_SHA256_DIGEST_SIZE) {
    return false;
  }

  uint8_t newSig[ndn_SHA256_DIGEST_SIZE];
  CryptoLite::digestSha256(input, inputLen, newSig);
  return std::memcmp(sig, newSig, ndn_SHA256_DIGEST_SIZE) == 0;
}

} // namespace ndn
