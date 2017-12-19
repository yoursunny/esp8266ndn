#ifndef ESP8266NDN_EC_KEY_HPP
#define ESP8266NDN_EC_KEY_HPP

#include "../ndn-cpp/lite/interest-lite.hpp"
#include "../ndn-cpp/lite/data-lite.hpp"

namespace ndn {

/** \brief Holds an EC private key and/or public key.
 */
class EcKey
{
public:
  EcKey(const uint8_t* pvtKey, const uint8_t* pubKey);

  /** \brief Sign input using SignatureSha256WithEcdsa.
   */
  bool
  sign(const uint8_t* input, size_t len, uint8_t* signature, size_t* sigLen) const;

  /** \brief Verify signature on input using SignatureSha256WithEcdsa.
   */
  bool
  verify(const uint8_t* input, size_t len, const uint8_t* signature, size_t sigLen) const;

private:
  const uint8_t* m_pvtKey;
  const uint8_t* m_pubKey;
};

} // namespace ndn

#endif // ESP8266NDN_EC_KEY_HPP
