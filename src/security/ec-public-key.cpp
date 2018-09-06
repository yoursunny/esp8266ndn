#include "ec-public-key.hpp"
#include "detail/ec-impl.hpp"
#include "../ndn-cpp/lite/util/crypto-lite.hpp"

namespace ndn {

EcPublicKey::EcPublicKey() = default;

EcPublicKey::EcPublicKey(const uint8_t bits[64])
{
  uint8_t bits1[65];
  bits1[0] = 0x04;
  memcpy_P(&bits1[1], bits, 64);
  this->import(bits1);
}

EcPublicKey::~EcPublicKey() = default;

bool
EcPublicKey::import(const uint8_t bits[65])
{
  m_impl.reset(new Impl(bits));
  return true;
}

bool
EcPublicKey::verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const
{
  if (!m_impl) {
    return false;
  }
  uint8_t hash[ndn_SHA256_DIGEST_SIZE];
  CryptoLite::digestSha256(input, inputLen, hash);
  return m_impl->verify(hash, sig, sigLen);
}

} // namespace ndn
