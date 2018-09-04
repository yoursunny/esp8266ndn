#include <StreamString.h>

static String
toHexString(const uint8_t* input, size_t len)
{
  StreamString os;
  for (size_t i = 0; i < len; ++i) {
    os.printf("%02X", input[i]);
  }
  return os;
}
