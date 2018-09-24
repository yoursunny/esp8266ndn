#ifndef ESP8266NDN_EC_IMPL_MBEDTLS_HPP
#define ESP8266NDN_EC_IMPL_MBEDTLS_HPP

#include <mbedtls/bignum.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/ecp.h>
#include "mbedtls-helper.hpp"

namespace ndn {
namespace detail {

class EcKeyImplBase
{
protected:
  EcKeyImplBase()
  {
    mbedtls_ecp_keypair_init(&m_key);
    mbedtls_ecp_group_load(&m_key.grp, MBEDTLS_ECP_DP_SECP256R1);
  }

  ~EcKeyImplBase()
  {
    mbedtls_ecp_keypair_free(&m_key);
  }

protected:
  mutable mbedtls_ecp_keypair m_key;
};

class EcPrivateKeyImpl : public EcKeyImplBase
{
public:
  explicit
  EcPrivateKeyImpl(const uint8_t bits[32])
  {
    // If bits come from PROGMEM, the next line will crash due to misaligned memory access.
    // However, ESP32 has no PROGMEM so this is fine.
    mbedtls_mpi_read_binary(&m_key.d, bits, 32);
  }

  int
  sign(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], uint8_t* sig) const
  {
    size_t sigLen;
    int res = mbedtls_ecdsa_write_signature(
      &m_key, MBEDTLS_MD_SHA256, hash, ndn_SHA256_DIGEST_SIZE, sig, &sigLen, &mbedRng, nullptr);
    return res == 0 ? static_cast<int>(sigLen) : 0;
  }
};

class EcPublicKeyImpl : public EcKeyImplBase
{
public:
  explicit
  EcPublicKeyImpl(const uint8_t bits[65])
  {
    // If bits come from PROGMEM, the next line will crash due to misaligned memory access.
    // However, ESP32 has no PROGMEM so this is fine.
    mbedtls_ecp_point_read_binary(&m_key.grp, &m_key.Q, bits, 65);
  }

  bool
  verify(const uint8_t hash[ndn_SHA256_DIGEST_SIZE], const uint8_t* sig, size_t sigLen) const
  {
    return mbedtls_ecdsa_read_signature(&m_key, hash, ndn_SHA256_DIGEST_SIZE, sig, sigLen) == 0;
  }
};

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_EC_IMPL_MBEDTLS_HPP
