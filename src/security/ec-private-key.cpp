#include "ec-private-key.hpp"
#include "detail/ec-impl.hpp"
#include "../ndn-cpp/lite/util/crypto-lite.hpp"

namespace ndn {

EcPrivateKey::EcPrivateKey(const uint8_t bits[32], const NameLite& keyName)
  : m_keyName(keyName)
{
  m_impl.reset(new Impl(bits));
}

EcPrivateKey::~EcPrivateKey() = default;

int
EcPrivateKey::sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const
{
  uint8_t hash[ndn_SHA256_DIGEST_SIZE];
  CryptoLite::digestSha256(input, inputLen, hash);
  return m_impl->sign(hash, sig);
}

ndn_Error
EcPrivateKey::setSignatureInfo(SignatureLite& signature) const
{
  signature.setType(ndn_SignatureType_Sha256WithEcdsaSignature);
  KeyLocatorLite& kl = signature.getKeyLocator();
  kl.setType(ndn_KeyLocatorType_KEYNAME);
  return kl.setKeyName(m_keyName);
}

} // namespace ndn
