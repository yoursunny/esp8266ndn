#include "ec-key.hpp"
#include "../micro-ecc/uECC.h"
#include "../ndn-cpp/c/encoding/tlv/tlv.h"
#include "../ndn-cpp/cryptosuite/sha256.h"
#include "logger.hpp"

namespace ndn {

enum {
  ASN1_SEQUENCE = 0x30,
  ASN1_INTEGER  = 0x02,
};

static int
rng(uint8_t* dest, unsigned size)
{
  for (uint8_t* end = dest + size; dest != end; ++dest) {
    *dest = static_cast<uint8_t>(random(256));
  }
  return 1;
}

EcKey::EcKey(const uint8_t* pvtKey, const uint8_t* pubKey)
  : m_pvtKey(pvtKey)
  , m_pubKey(pubKey)
{
  uECC_set_rng(&rng);
}

static inline const uint8_t*
computeSha256Hash(const uint8_t* input, size_t len)
{
  Sha256.init();
  for (size_t i = 0; i < len; ++i) {
    Sha256.write(input[i]);
  }
  return Sha256.result();
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
EcKey::sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const
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

bool
EcKey::verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const
{
  if (m_pubKey == nullptr) {
    return false;
  }

  uint8_t rawSig[uECC_BYTES * 2];
  if (!decodeSignatureBits(sig, sigLen, rawSig)) {
    return false;
  }

  const uint8_t* hash = computeSha256Hash(input, inputLen);
  return uECC_verify(m_pubKey, hash, rawSig);
}

} // namespace ndn
