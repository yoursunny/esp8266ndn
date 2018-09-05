#ifndef ESP8266NDN_EC_IMPL_BEARSSL_HPP
#define ESP8266NDN_EC_IMPL_BEARSSL_HPP

#include "uECC.h"
#include "../../ndn-cpp/lite/util/crypto-lite.hpp"

#include <cstring>
#include <pgmspace.h>

namespace ndn {
namespace detail {

enum {
  ASN1_SEQUENCE = 0x30,
  ASN1_INTEGER  = 0x02,
};

inline int
rng(uint8_t* dest, unsigned size)
{
  CryptoLite::generateRandomBytes(dest, size);
  return 1;
}

/** \brief Determine ASN1 length of integer at integer[0:32].
 */
inline int
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

/** \brief Write ASN1 integer from integer[0:32] to output..retval; buffers may overlap.
 */
inline uint8_t*
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

/** \brief Encode 64-octet raw signature at sig[8:72] as DER at sig[0:retval].
 */
inline int
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

/** \brief Read ASN1 integer at input..end into output[0:32].
 *  \return pointer past end of ASN1 integer, or nullptr if failure.
 */
inline const uint8_t*
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

/** \brief Decode DER-encoded ECDSA signature into 64-octet raw signature.
 */
inline bool
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

class EcPrivateKeyImpl
{
public:
  explicit
  EcPrivateKeyImpl(const uint8_t bits[65])
  {
    uECC_set_rng(&rng);
    memcpy_P(m_bits, bits, 32);
  }

  int
  sign(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], uint8_t* sig) const
  {
    int res = uECC_sign(m_bits, hash, sig + 8);
    return res != 0 && encodeSignatureBits(sig);
  }

private:
  uint8_t m_bits[32];
};

class EcPublicKeyImpl
{
public:
  explicit
  EcPublicKeyImpl(const uint8_t bits[65])
  {
    memcpy_P(m_bits, &bits[1], 64);
  }

  bool
  verify(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], const uint8_t* sig, size_t sigLen) const
  {
    uint8_t rawSig[uECC_BYTES * 2];
    return decodeSignatureBits(sig, sigLen, rawSig) && uECC_verify(m_bits, hash, rawSig);
  }

private:
  uint8_t m_bits[64];
};

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_EC_IMPL_BEARSSL_HPP
