#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "autoconfig.hpp"
#include "../core/logger.hpp"

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <HTTPClient.h>
#include <WiFi.h>
#endif

#define LOG(...) LOGGER(AutoConfig, __VA_ARGS__)

namespace esp8266ndn {

IPAddress
queryFchService(String serviceUri)
{
  WiFiClient tcp;
  HTTPClient http;
  http.begin(tcp, serviceUri);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    LOG(serviceUri << F(" error: ") << httpCode);
    return IPAddress(0, 0, 0, 0);
  }
  String response = http.getString();
  LOG(serviceUri << F(" response: ") << response);

  IPAddress ip;
  if (!WiFi.hostByName(response.c_str(), ip)) {
    LOG(F("DNS error"));
    return IPAddress(0, 0, 0, 0);
  }
  LOG(F("DNS resolved to: ") << ip);
  return ip;
}

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
