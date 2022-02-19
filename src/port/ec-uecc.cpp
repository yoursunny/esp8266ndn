#include "ec-uecc.hpp"
#ifdef ESP8266NDN_PORT_EC_UECC

#include "random.hpp"

#include <algorithm>
#include <cstring>

namespace esp8266ndn {
namespace ndnph_port_uecc {

class UeccSetRng
{
public:
  static void once()
  {
    static UeccSetRng instance;
  }

private:
  UeccSetRng()
  {
    uECC_set_rng(rng);
  }

  static int rng(uint8_t* dest, unsigned size)
  {
    return ndnph_port::RandomSource::generate(dest, size);
  }
};

namespace {

enum
{
  ASN1_SEQUENCE = 0x30,
  ASN1_INTEGER = 0x02,
};

/** @brief Determine ASN1 length of integer at integer[0:32]. */
inline int
determineAsn1IntLength(const uint8_t* integer)
{
  if ((integer[0] & 0x80) != 0x00) {
    return 33;
  }

  int len = 32;
  for (int i = 0; i < 31; ++i) {
    if (((static_cast<uint16_t>(integer[i]) << 8) | (integer[i + 1] & 0x80)) != 0x0000) {
      break;
    }
    --len;
  }
  return len;
}

/** @brief Write ASN1 integer from integer[0:32] to output..retval; buffers may overlap. */
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

/** @brief Encode 64-octet raw signature at sig[8:72] as DER at sig[0:retval]. */
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

/**
 * @brief Read ASN1 integer at input..end into output[0:32].
 * @return pointer past end of ASN1 integer, or nullptr if failure.
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

/** @brief Decode DER-encoded ECDSA signature into 64-octet raw signature. */
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

} // anonymous namespace

bool
Ec::PrivateKey::import(const uint8_t key[Curve::PubLen::value])
{
  std::copy_n(key, sizeof(m_key), m_key);
  return true;
}

ssize_t
Ec::PrivateKey::sign(const uint8_t digest[uECC_BYTES], uint8_t sig[Curve::MaxSigLen::value]) const
{
  UeccSetRng::once();
  bool ok = uECC_sign(m_key, digest, &sig[8]);
  if (!ok) {
    return -1;
  }
  return encodeSignatureBits(sig);
}

bool
Ec::PublicKey::import(const uint8_t key[Curve::PubLen::value])
{
  if (key[0] != 0x04) {
    return false;
  }
  std::copy_n(&key[1], sizeof(m_key), m_key);
  return uECC_valid_public_key(m_key);
}

bool
Ec::PublicKey::verify(const uint8_t digest[uECC_BYTES], const uint8_t* sig, size_t sigLen) const
{
  uint8_t rawSig[uECC_BYTES * 2];
  if (!decodeSignatureBits(sig, sigLen, rawSig)) {
    return false;
  }
  return uECC_verify(m_key, digest, rawSig);
}

bool
Ec::generateKey(uint8_t pvt[Curve::PvtLen::value], uint8_t pub[Curve::PubLen::value])
{
  UeccSetRng::once();
  pub[0] = 0x04;
  return uECC_make_key(&pub[1], pvt);
}

} // namespace ndnph_port_uecc
} // namespace esp8266ndn

#endif // ESP8266NDN_PORT_EC_UECC
