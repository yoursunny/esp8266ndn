#ifndef ESP8266NDN_EC_KEY_HPP
#define ESP8266NDN_EC_KEY_HPP

#include "../ndn-cpp/lite/interest-lite.hpp"
#include "../ndn-cpp/lite/data-lite.hpp"

namespace ndn {

#define NDNECKEY_MAXSIGLEN 72

/** \brief Holds an EC private key and/or public key.
 */
class EcKey
{
public:
  EcKey(const uint8_t* pvtKey, const uint8_t* pubKey);

  /** \brief Sign input using SignatureSha256WithEcdsa.
   *  \param input signed portion
   *  \param inputLen input length
   *  \param[out] sig signature bits
   *  \return SignatureValue total length
   *  \retval 0 error
   */
  int
  sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const;

  /** \brief Verify signature on input using SignatureSha256WithEcdsa.
   *  \param input signed portion
   *  \param inputLen input length
   *  \param sig signature bits
   *  \param sigLen SignatureValue total length
   */
  bool
  verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const;

private:
  const uint8_t* m_pvtKey;
  const uint8_t* m_pubKey;
};

} // namespace ndn

#endif // ESP8266NDN_EC_KEY_HPP
