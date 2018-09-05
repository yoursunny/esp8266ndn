#ifndef ESP8266NDN_EC_PUBLIC_KEY_HPP
#define ESP8266NDN_EC_PUBLIC_KEY_HPP

#include "public-key.hpp"

namespace ndn {

/** \brief Holds an EC public key (curve secp256r1) for SignatureSha256WithEcdsa signature type.
 */
class EcPublicKey : public PublicKey
{
public:
  /** \brief Construct EC public key from key bits.
   *  \param bits uncompressed points in standard format without leading 0x04 prefix;
   *              may come from PGMSPACE, will be copied
   */
  explicit
  EcPublicKey(const uint8_t bits[64]);

  bool
  verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const final;

private:
  uint8_t m_pubKey[64];
};

} // namespace ndn

#endif // ESP8266NDN_EC_PUBLIC_KEY_HPP
