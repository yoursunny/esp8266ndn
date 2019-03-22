#include "test-common.hpp"

void setup()
{
  Serial.begin(115200);
  Serial.println();
  ndn::setLogOutput(Serial);
}

void loop()
{
  TestRunner::run();
}

#ifdef ARDUINO_ARCH_NRF52
namespace std {
// needed by std::vector<> but not in library, mark as 'weak' in case they add it
void __attribute__((weak)) __throw_length_error(char const*) { while(true) {} }
} // namespace std
#endif
