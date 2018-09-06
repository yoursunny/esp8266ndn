#ifndef ESP8266NDN_ECKEYGEN_IMPL_MICROECC_HPP
#define ESP8266NDN_ECKEYGEN_IMPL_MICROECC_HPP

#include "microecc.hpp"
#include "../ec-private-key.hpp"
#include "../ec-public-key.hpp"

namespace ndn {
namespace detail {

inline bool
makeEcKey(EcPrivateKey& pvt, EcPublicKey& pub)
{
  setMicroEccRng();
  uint8_t pvtBits[32];
  uint8_t pubBits[65] = {0x04};
  int res = uECC_make_key(&pubBits[1], pvtBits);
  return res != 0 && pvt.import(pvtBits) && pub.import(pubBits);
}

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_ECKEYGEN_IMPL_MICROECC_HPP
