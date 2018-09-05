#ifndef ESP8266NDN_EC_PRIVATE_KEY_HPP
#define ESP8266NDN_EC_PRIVATE_KEY_HPP

#include "private-key.hpp"
#include <memory>

namespace ndn {

/** \brief Holds an EC private key (curve secp256r1) for SignatureSha256WithEcdsa signature type.
 */
class EcPrivateKey : public PrivateKey
{
public:
  /** \brief Construct EC private key from key bits.
   *  \param bits uncompressed points in standard format;
   *              may come from PROGMEM, will be copied
   *  \param keyName certificate name; caller must retain memory
   */
  EcPrivateKey(const uint8_t bits[32], const NameLite& keyName);

  ~EcPrivateKey();

  size_t
  getMaxSigLength() const final
  {
    return MAX_SIG_LENGTH;
  }

  int
  sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const final;

  ndn_Error
  setSignatureInfo(SignatureLite& signature) const final;

public:
  static constexpr size_t MAX_SIG_LENGTH = 72;

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
  const NameLite& m_keyName;
};

} // namespace ndn

#endif // ESP8266NDN_EC_PRIVATE_KEY_HPP
