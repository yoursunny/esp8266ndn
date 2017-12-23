#ifndef ESP8266NDN_PRIVATE_KEY_HPP
#define ESP8266NDN_PRIVATE_KEY_HPP

#include <cinttypes>
#include <cstddef>

namespace ndn {

/** \brief Represents a private key.
 */
class PrivateKey
{
public:
  virtual
  ~PrivateKey() = default;

  /** \brief Get maximum length of signature bits.
   */
  virtual size_t
  getMaxSigLength() const = 0;

  /** \brief Sign input with this key.
   *  \param input signed portion
   *  \param inputLen input length
   *  \param[out] sig signature bits, must have space for \p getMaxSigLength()
   *  \return length of signature bits
   *  \retval 0 error
   */
  virtual int
  sign(const uint8_t* input, size_t inputLen, uint8_t* sig) const = 0;
};

} // namespace ndn

#endif // ESP8266NDN_PRIVATE_KEY_HPP
