#ifndef ESP8266NDN_EC_PRIVATE_KEY_HPP
#define ESP8266NDN_EC_PRIVATE_KEY_HPP

#include "private-key.hpp"

namespace ndn {

/** \brief Holds an EC private key (curve secp256r1) for SignatureSha256WithEcdsa signature type.
 */
class EcPrivateKey : public PrivateKey
{
public:
  /** \brief Construct EC private key from key bits.
   *  \param bits uncompressed points in standard format;
   *              caller must retain memory of these bits
   */
  EcPrivateKey(const uint8_t bits[32]);

  size_t
  getMaxSigLength() const final
  {
    return MAX_SIG_LENGTH;
  }

  int
  sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const final;

public:
  static constexpr size_t MAX_SIG_LENGTH = 72;

private:
  const uint8_t* m_pvtKey;
};

} // namespace ndn

#endif // ESP8266NDN_EC_PRIVATE_KEY_HPP
