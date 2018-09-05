#ifndef ESP8266NDN_EC_IMPL_BEARSSL_HPP
#define ESP8266NDN_EC_IMPL_BEARSSL_HPP

#include "../../ndn-cpp/c/common.h"

#include <pgmspace.h>
#include <bearssl/bearssl_hash.h>
#include <bearssl/bearssl_ec.h>

namespace ndn {
namespace detail {

class EcPrivateKeyImpl
{
public:
  explicit
  EcPrivateKeyImpl(const uint8_t bits[32])
  {
    memcpy_P(m_bits, bits, sizeof(m_bits));
    m_key.curve = BR_EC_secp256r1;
    m_key.x = m_bits;
    m_key.xlen = 32;
  }

  int
  sign(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], uint8_t* sig) const
  {
    return br_ecdsa_i31_sign_asn1(&br_ec_p256_m31, &br_sha256_vtable, hash, &m_key, sig);
  }

private:
  uint8_t m_bits[32];
  br_ec_private_key m_key;
};

class EcPublicKeyImpl
{
public:
  explicit
  EcPublicKeyImpl(const uint8_t bits[64])
  {
    m_bits[0] = 0x04;
    memcpy_P(&m_bits[1], bits, 64);
    m_key.curve = BR_EC_secp256r1;
    m_key.q = m_bits;
    m_key.qlen = 65;
  }

  bool
  verify(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], const uint8_t* sig, size_t sigLen) const
  {
    return br_ecdsa_i31_vrfy_asn1(&br_ec_p256_m31, hash, ndn_SHA256_DIGEST_SIZE, &m_key, sig, sigLen);
  }

private:
  uint8_t m_bits[65];
  br_ec_public_key m_key;
};

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_EC_IMPL_BEARSSL_HPP
