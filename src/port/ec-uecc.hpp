#ifndef ESP8266NDN_PORT_EC_UECC_HPP
#define ESP8266NDN_PORT_EC_UECC_HPP

#include "choose.h"

#ifdef ESP8266NDN_PORT_EC_UECC

#include "../vendor/uECC.h"
#include <sys/types.h>
#include <type_traits>

namespace esp8266ndn {
namespace ndnph_port_uecc {

/** @brief ECDSA P-256, implemented with micro-ecc. */
class Ec
{
public:
  struct Curve
  {
    using PvtLen = std::integral_constant<size_t, uECC_BYTES>;
    using PubLen = std::integral_constant<size_t, 1 + uECC_BYTES * 2>;
    using MaxSigLen = std::integral_constant<size_t, 9 + uECC_BYTES * 2>;
  };

  class PrivateKey
  {
  public:
    bool import(const uint8_t key[Curve::PubLen::value]);

    ssize_t sign(const uint8_t digest[uECC_BYTES], uint8_t sig[Curve::MaxSigLen::value]) const;

  private:
    uint8_t m_key[uECC_BYTES];
  };

  class PublicKey
  {
  public:
    bool import(const uint8_t[Curve::PubLen::value]);

    bool verify(const uint8_t digest[uECC_BYTES], const uint8_t* sig, size_t sigLen) const;

  private:
    uint8_t m_key[2 * uECC_BYTES];
  };

  static bool generateKey(uint8_t pvt[Curve::PvtLen::value], uint8_t pub[Curve::PubLen::value]);
};

} // namespace ndnph_port_uecc
} // namespace esp8266ndn

namespace ndnph {
namespace port {
using Ec = esp8266ndn::ndnph_port_uecc::Ec;
} // namespace port
} // namespace ndnph

#endif // ESP8266NDN_PORT_EC_UECC
#endif // ESP8266NDN_PORT_EC_UECC_HPP
