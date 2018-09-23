#include <esp8266ndn.h>

#include <AUnitVerbose.h>
#include <StreamString.h>

using namespace aunit;

inline String
toHexString(const uint8_t* input, size_t len)
{
  StreamString os;
  for (size_t i = 0; i < len; ++i) {
    os.printf("%02X", input[i]);
  }
  return os;
}
