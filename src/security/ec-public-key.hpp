#ifndef ESP8266NDN_EC_PUBLIC_KEY_HPP
#define ESP8266NDN_EC_PUBLIC_KEY_HPP

#include "public-key.hpp"
#include <memory>

namespace ndn {

/** \brief Holds an EC public key (curve secp256r1) for SignatureSha256WithEcdsa signature type.
 */
class EcPublicKey : public PublicKey
{
public:
  /** \brief Construct EC public key from key bits.
   *  \param bits uncompressed points in standard format without leading 0x04 prefix;
   *              may come from PROGMEM, will be copied
   */
  explicit
  EcPublicKey(const uint8_t bits[64]);

  ~EcPublicKey();

  bool
  verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const final;

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // ESP8266NDN_EC_PUBLIC_KEY_HPP
