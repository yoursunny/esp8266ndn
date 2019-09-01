#ifndef ESP8266NDN_LOGGING_HPP
#define ESP8266NDN_LOGGING_HPP

#include <Print.h>

namespace ndn {

Print&
getLogOutput();

void
setLogOutput(Print& output);

/** \brief Print a buffer in hexadecimal.
 */
class PrintHex : public Printable
{
public:
  explicit
  PrintHex(const uint8_t* buf, size_t len);

  size_t
  printTo(Print& p) const override;

private:
  const uint8_t* m_buf;
  size_t m_len;
};

} // namespace ndn

#endif // ESP8266NDN_LOGGING_HPP
