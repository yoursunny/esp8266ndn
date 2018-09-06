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
