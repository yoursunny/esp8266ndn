#include "ec-private-key.hpp"
#include "micro-ecc/uECC.h"
#include "detail/asn1.hpp"
#include "sha256.hpp"
#include <Arduino.h>

namespace ndn {

static int
rng(uint8_t* dest, unsigned size)
{
  for (uint8_t* end = dest + size; dest != end; ++dest) {
    *dest = static_cast<uint8_t>(random(256));
  }
  return 1;
}

EcPrivateKey::EcPrivateKey(const uint8_t bits[32])
  : m_pvtKey(bits)
{
  uECC_set_rng(&rng);
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

int
EcPrivateKey::sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const
{
  if (m_pvtKey == nullptr) {
    return 0;
  }

  const uint8_t* hash = computeSha256Hash(input, inputLen);
  int res = uECC_sign(m_pvtKey, hash, sig + 8);
  if (res == 0) {
    return 0;
  }

  return encodeSignatureBits(sig);
}

} // namespace ndn
