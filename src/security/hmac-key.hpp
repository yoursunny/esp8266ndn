#ifndef ESP8266NDN_HMAC_KEY_HPP
#define ESP8266NDN_HMAC_KEY_HPP

#include "private-key.hpp"
#include "public-key.hpp"
#include <memory>

namespace ndn {
namespace detail {
class HmacImpl;
} // namespace detail

/** \brief Holds an HMAC key for SignatureHmacWithSha256 signature type.
 */
class HmacKey : public PrivateKey, public PublicKey
{
public:
  /** \brief Construct HMAC key from shared secret key bits.
   *  \param secret shared secret; caller must retain memory
   *  \param secretLen length of shared secret
   */
  HmacKey(const uint8_t* secret, size_t secretLen);

  ~HmacKey();

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

private:
  using Impl = detail::HmacImpl;
  std::unique_ptr<Impl> m_impl;
  uint8_t m_keyDigest[ndn_SHA256_DIGEST_SIZE];
};

} // namespace ndn

#endif // ESP8266NDN_HMAC_KEY_HPP
