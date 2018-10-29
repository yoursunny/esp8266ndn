#ifndef ESP8266NDN_EC_PUBLIC_KEY_HPP
#define ESP8266NDN_EC_PUBLIC_KEY_HPP

#include "public-key.hpp"
#include "../ndn-cpp/lite/data-lite.hpp"
#include <memory>

namespace ndn {
namespace detail {
class EcPublicKeyImpl;
} // namespace detail

/** \brief Holds an EC public key (curve secp256r1) for SignatureSha256WithEcdsa signature type.
 */
class EcPublicKey : public PublicKey
{
public:
  EcPublicKey();

  ~EcPublicKey();

  /** \brief Import key bits.
   *  \param bits uncompressed points in standard format, starting with 0x04 prefix;
   *              may come from PROGMEM, will be copied
   */
  bool
  import(const uint8_t bits[65]);

  /** \brief Import key from certificate.
   *  \param data NDN Certificate V2 data
   */
  bool
  importCert(const DataLite& data);

  bool
  verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const final;

private:
  using Impl = detail::EcPublicKeyImpl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // ESP8266NDN_EC_PUBLIC_KEY_HPP
