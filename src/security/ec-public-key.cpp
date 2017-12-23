#include "ec-public-key.hpp"
#include "detail/asn1.hpp"
#include "micro-ecc/uECC.h"
#include "../ndn-cpp/c/util/crypto.h"
#include <cstring>

namespace ndn {

EcPublicKey::EcPublicKey(const uint8_t bits[64])
  : m_pubKey(bits)
{
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
EcPublicKey::verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const
{
  uint8_t rawSig[uECC_BYTES * 2];
  if (!decodeSignatureBits(sig, sigLen, rawSig)) {
    return false;
  }

  uint8_t hash[ndn_SHA256_DIGEST_SIZE];
  ndn_digestSha256(input, inputLen, hash);

  return uECC_verify(m_pubKey, hash, rawSig);
}

} // namespace ndn
