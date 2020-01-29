#include <esp8266ndn.h>

#include <Arduino.h>
#include <ArduinoUnit.h>

test(Simple)
{
  assertEqual(1 + 2, 3);
}

void
setup()
{
  Serial.begin(115200);
}

void
loop()
{
  Test::run();
}
