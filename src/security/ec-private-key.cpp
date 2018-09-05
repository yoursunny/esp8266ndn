#include "ec-private-key.hpp"
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

class EcPrivateKey::Impl
{
public:
  explicit
  Impl(const uint8_t bits[32]);

  int
  sign(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], uint8_t* sig) const;

private:
  uint8_t m_bits[32];
#if defined(ESP8266)
  br_ec_private_key m_key;
#endif
};

#if defined(ESP8266)

EcPrivateKey::Impl::Impl(const uint8_t bits[32])
{
  memcpy_P(m_bits, bits, sizeof(m_bits));
  m_key.curve = BR_EC_secp256r1;
  m_key.x = m_bits;
  m_key.xlen = 65;
}

int
EcPrivateKey::Impl::sign(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], uint8_t* sig) const
{
  return br_ecdsa_i31_sign_asn1(&br_ec_p256_m31, &br_sha256_vtable, hash, &m_key, sig);
}

#elif defined(ESP32)

static int
rng(uint8_t* dest, unsigned size)
{
  CryptoLite::generateRandomBytes(dest, size);
  return 1;
}

// Determine ASN1 length of integer at integer[0:32].
static inline int
determineAsn1IntLength(const uint8_t* integer)
{
  if ((integer[0] & 0x80) != 0x00) {
    return 33;
  }

  int len = 32;
  for (int i = 0; i < 31; ++i) {
    if (((integer[i] << 8) | (integer[i + 1] & 0x80)) != 0x0000) {
      break;
    }
    --len;
  }
  return len;
}

// Write ASN1 integer from integer[0:32] to output..retval; buffers may overlap.
static inline uint8_t*
writeAsn1Int(uint8_t* output, const uint8_t* integer, int length)
{
  *(output++) = ASN1_INTEGER;
  *(output++) = static_cast<uint8_t>(length);

  if (length == 33) {
    *(output++) = 0x00;
    memmove(output, integer, 32);
    return output + 32;
  }

  memmove(output, integer + 32 - length, length);
  return output + length;
}

// Encode 64-octet raw signature at sig[8:72] as DER at sig[0:retval].
static inline int
encodeSignatureBits(uint8_t* sig)
{
  const uint8_t* begin = sig;
  const uint8_t* r = sig + 8;
  const uint8_t* s = r + 32;
  int rLength = determineAsn1IntLength(r);
  int sLength = determineAsn1IntLength(s);

  *(sig++) = ASN1_SEQUENCE;
  *(sig++) = 2 + rLength + 2 + sLength;
  sig = writeAsn1Int(sig, r, rLength);
  sig = writeAsn1Int(sig, s, sLength);

  return sig - begin;
}

EcPrivateKey::Impl::Impl(const uint8_t bits[32])
{
  uECC_set_rng(&rng);
  memcpy_P(m_bits, bits, sizeof(m_bits));
}

int
EcPrivateKey::Impl::sign(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], uint8_t* sig) const
{
  int res = uECC_sign(m_bits, hash, sig + 8);
  if (res == 0) {
    return 0;
  }
  return encodeSignatureBits(sig);
}

#endif

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
