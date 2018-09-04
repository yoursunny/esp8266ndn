#include <esp8266ndn.h>
#include <AUnitVerbose.h>
using namespace aunit;
#include "helper.hpp"

#include "core_uri_test.hpp"
#include "security_test.hpp"

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
