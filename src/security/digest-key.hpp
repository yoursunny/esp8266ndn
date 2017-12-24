#ifndef ESP8266NDN_DIGEST_KEY_HPP
#define ESP8266NDN_DIGEST_KEY_HPP

#include "private-key.hpp"
#include "public-key.hpp"

namespace ndn {

/** \brief Selects DigestSha256 signature type.
 */
class DigestKey : public PrivateKey, public PublicKey
{
public:
  size_t
  getMaxSigLength() const final
  {
    return ndn_SHA256_DIGEST_SIZE;
  }

  int
  sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const final;

  ndn_Error
  setSignatureInfo(SignatureLite& signature) const final;

  bool
  verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const final;
};

} // namespace ndn

#endif // ESP8266NDN_DIGEST_KEY_HPP
