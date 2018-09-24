#ifndef ESP8266NDN_ECKEYGEN_IMPL_MBEDTLS_HPP
#define ESP8266NDN_ECKEYGEN_IMPL_MBEDTLS_HPP

#include "../ec-private-key.hpp"
#include "../ec-public-key.hpp"
#include <mbedtls/bignum.h>
#include <mbedtls/ecp.h>
#include "mbedtls-helper.hpp"

namespace ndn {
namespace detail {

inline bool
makeEcKey(EcPrivateKey& pvt, EcPublicKey& pub)
{
  mbedtls_ecp_keypair key;
  mbedtls_ecp_keypair_init(&key);
  int res = mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, &key, &mbedRng, nullptr);
  if (res != 0) {
    return false;
  }

  uint8_t pvtBits[32];
  uint8_t pubBits[65];
  res = mbedtls_mpi_write_binary(&key.d, pvtBits, sizeof(pvtBits));
  size_t pubLen;
  int res2 = mbedtls_ecp_point_write_binary(&key.grp, &key.Q, MBEDTLS_ECP_PF_UNCOMPRESSED,
                                            &pubLen, pubBits, sizeof(pubBits));
  mbedtls_ecp_keypair_free(&key);
  return res == 0 && res2 == 0 && pubLen == sizeof(pubBits) &&
         pvt.import(pvtBits) && pub.import(pubBits);
}

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_ECKEYGEN_IMPL_MBEDTLS_HPP
