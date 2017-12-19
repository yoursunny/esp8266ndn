#include "ec-key.hpp"
#include "../micro-ecc/uECC.h"
#include "../ndn-cpp/c/encoding/tlv/tlv.h"
#include "../ndn-cpp/cryptosuite/sha256.h"
#include "logger.hpp"

namespace ndn {

EcKey::EcKey(const uint8_t* pvtKey, const uint8_t* pubKey)
  : m_pvtKey(pvtKey)
  , m_pubKey(pubKey)
{
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

bool
EcKey::sign(const uint8_t* input, size_t len, uint8_t* signature, size_t* sigLen) const
{
  return false; // not implemented
}

// Decode SignatureValue element of DER-encoded ECDSA signature into 64-octet raw signature.
static inline bool
decodeSignatureValue(const uint8_t* input, size_t len, uint8_t* sig)
{
  enum {
    ASN1_SEQUENCE = 0x30,
    ASN1_INTEGER  = 0x02
  };

  memset(sig, 0, uECC_BYTES * 2);
  const uint8_t* end = input + len;

  if (input == end || *(input++) != ndn_Tlv_SignatureValue)
    return false;
  if (input == end)
    return false;
  uint8_t tlvLength = *(input++);
  if (input + tlvLength != end)
    return false;

  if (input == end || *(input++) != ASN1_SEQUENCE)
    return false;
  if (input == end)
    return false;
  uint8_t seqLength = *(input++);
  if (input + seqLength != end)
    return false;

  if (input == end || *(input++) != ASN1_INTEGER)
    return false;
  if (input == end)
    return false;
  uint8_t rLength = *(input++);
  if (input + rLength >= end)
    return false;
  uint8_t rCopyLength = rLength < uECC_BYTES ? rLength : uECC_BYTES;
  memcpy(sig + uECC_BYTES - rCopyLength, input + rLength - rCopyLength, rCopyLength);
  input += rLength;

  if (input == end || *(input++) != ASN1_INTEGER)
    return false;
  if (input == end)
    return false;
  uint8_t sLength = *(input++);
  if (input + sLength != end)
    return false;
  uint8_t sCopyLength = sLength < uECC_BYTES ? sLength : uECC_BYTES;
  memcpy(sig + uECC_BYTES + uECC_BYTES - sCopyLength, input + sLength - sCopyLength, sCopyLength);
  input += sLength;
  if (input != end)
    return false;

  return true;
}

bool
EcKey::verify(const uint8_t* input, size_t len, const uint8_t* signature, size_t sigLen) const
{
  if (m_pubKey == nullptr) {
    return false;
  }

  uint8_t rawSig[uECC_BYTES * 2];
  if (!decodeSignatureValue(signature, sigLen, rawSig)) {
    return false;
  }

  const uint8_t* hash = computeSha256Hash(input, len);
  return uECC_verify(m_pubKey, hash, rawSig);
}

} // namespace ndn
