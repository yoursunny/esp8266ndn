#include "autoconfig.hpp"
#include "../core/logger.hpp"

#if defined(ARDUINO_ARCH_ESP8266)
#define HAVE_HTTPCLIENT
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#define HAVE_HTTPCLIENT
#include <HTTPClient.h>
#include <WiFi.h>
#endif

#define LOG(...) LOGGER(AutoConfig, __VA_ARGS__)

namespace esp8266ndn {

IPAddress
queryFchService(String serviceUri)
{
#ifdef HAVE_HTTPCLIENT
  WiFiClient tcp;
  HTTPClient http;
  http.begin(tcp, serviceUri);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    LOG(serviceUri << F(" error: ") << httpCode);
    return INADDR_NONE;
  }
  String response = http.getString();
  LOG(serviceUri << F(" response: ") << response);

  IPAddress ip;
  if (!WiFi.hostByName(response.c_str(), ip)) {
    LOG(F("DNS error"));
    return INADDR_NONE;
  }
  LOG(F("DNS resolved to: ") << ip);
  return ip;
#else  // HAVE_HTTPCLIENT
  return INADDR_NONE;
#endif // HAVE_HTTPCLIENT
}

} // namespace esp8266ndn
