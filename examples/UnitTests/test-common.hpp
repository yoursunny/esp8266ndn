#include <esp8266ndn.h>

#include <AUnitVerbose.h>

using namespace aunit;

class StringPrint : public Print
{
public:
  size_t
  write(uint8_t ch) override
  {
    str.concat(static_cast<char>(ch));
    return 1;
  }

public:
  String str;
};

inline String
toHexString(const uint8_t* input, size_t len)
{
  StringPrint os;
  for (size_t i = 0; i < len; ++i) {
    os.printf("%02X", input[i]);
  }
  return os.str;
}
