#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)

#include "autoconfig.hpp"
#include "../core/logger.hpp"

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)
#include <HTTPClient.h>
#include <WiFi.h>
#endif

#define LOG(...) LOGGER(AutoConfig, __VA_ARGS__)

namespace esp8266ndn {

FchResponse
fchQuery(ESP8266NDN_NetworkClient& client, String serviceUri) {
  FchResponse res;
  HTTPClient http;
  http.begin(client, serviceUri);

  int status = http.GET();
  if (status != HTTP_CODE_OK) {
    LOG(serviceUri << F(" error: ") << status);
    return res;
  }

  String body = http.getString();
  body.trim();
  LOG(serviceUri << F(" body: ") << body);

  if (!ESP8266NDN_Network.hostByName(body.c_str(), res.ip)) {
    LOG(F("DNS error"));
    return res;
  }
  LOG(F("DNS resolved to: ") << res.ip);
  res.ok = true;
  return res;
}

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
