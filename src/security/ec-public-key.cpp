#include "ec-public-key.hpp"
#include "../ndn-cpp/lite/util/crypto-lite.hpp"
#include <pgmspace.h>

#if defined(ESP8266)
#include <bearssl/bearssl_hash.h>
#include <bearssl/bearssl_ec.h>
#elif defined(ESP32)
#include "detail/asn1.hpp"
#include "micro-ecc/uECC.h"
#include <cstring>
#endif

namespace ndn {

class EcPublicKey::Impl
{
public:
  explicit
  Impl(const uint8_t bits[64]);

  bool
  verify(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], const uint8_t* sig, size_t sigLen) const;

private:
  uint8_t m_bits[65];
#if defined(ESP8266)
  br_ec_public_key m_key;
#endif
};

#if defined(ESP8266)

EcPublicKey::Impl::Impl(const uint8_t bits[64])
{
  m_bits[0] = 0x04;
  memcpy_P(&m_bits[1], bits, 64);
  m_key.curve = BR_EC_secp256r1;
  m_key.q = m_bits;
  m_key.qlen = sizeof(m_bits);
}

bool
EcPublicKey::Impl::verify(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], const uint8_t* sig, size_t sigLen) const
{
  return br_ecdsa_i31_vrfy_asn1(&br_ec_p256_m31, hash, ndn_SHA256_DIGEST_SIZE, &m_key, sig, sigLen);
}

#elif defined(ESP32)

// Read ASN1 integer at input..end into output[0:32].
// Return pointer past end of ASN1 integer, or nullptr if failure.
static inline const uint8_t*
readAsn1Int(const uint8_t* input, const uint8_t* end, uint8_t* output)
{
  if (input == end || *(input++) != ASN1_INTEGER)
    return nullptr;

  uint8_t length = (input == end) ? 0 : *(input++);
  if (length == 0 || input + length > end)
    return nullptr;

  if (length == 33) {
    --length;
    ++input;
  }
  memcpy(output + 32 - length, input, length);
  return input + length;
}

// Decode DER-encoded ECDSA signature into 64-octet raw signature.
static inline bool
decodeSignatureBits(const uint8_t* input, size_t len, uint8_t* decoded)
{
  memset(decoded, 0, uECC_BYTES * 2);
  const uint8_t* end = input + len;

  if (input == end || *(input++) != ASN1_SEQUENCE)
    return false;
  if (input == end)
    return false;
  uint8_t seqLength = *(input++);
  if (input + seqLength != end)
    return false;

  input = readAsn1Int(input, end, decoded + 0);
  input = readAsn1Int(input, end, decoded + 32);
  return input == end;
}

EcPublicKey::Impl::Impl(const uint8_t bits[64])
{
  memcpy_P(m_bits, bits, 64);
}

bool
EcPublicKey::Impl::verify(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], const uint8_t* sig, size_t sigLen) const
{
  uint8_t rawSig[uECC_BYTES * 2];
  if (!decodeSignatureBits(sig, sigLen, rawSig)) {
    return false;
  }

  return uECC_verify(m_bits, hash, rawSig);
}

#endif

EcPublicKey::EcPublicKey(const uint8_t bits[64])
{
  m_impl.reset(new Impl(bits));
}

EcPublicKey::~EcPublicKey() = default;

bool
EcPublicKey::verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const
{
  uint8_t hash[ndn_SHA256_DIGEST_SIZE];
  CryptoLite::digestSha256(input, inputLen, hash);
  return m_impl->verify(hash, sig, sigLen);
}

} // namespace ndn
