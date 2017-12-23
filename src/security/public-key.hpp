#ifndef ESP8266NDN_PUBLIC_KEY_HPP
#define ESP8266NDN_PUBLIC_KEY_HPP

#include <cinttypes>
#include <cstddef>

namespace ndn {

/** \brief Represents a public key.
 */
class PublicKey
{
public:
  virtual
  ~PublicKey() = default;

  /** \brief Verify signature on input with this key.
   *  \param input signed portion
   *  \param inputLen input length
   *  \param sig signature bits
   *  \param sigLen length of signature bits
   */
  virtual bool
  verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const = 0;
};

} // namespace ndn

#endif // ESP8266NDN_PUBLIC_KEY_HPP
