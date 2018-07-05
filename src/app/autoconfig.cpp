#include "autoconfig.hpp"
#include "../core/logger.hpp"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#define AUTOCONFIG_DBG(...) DBG(AutoConfig, __VA_ARGS__)

namespace ndn {

IPAddress
queryFchService(String serviceUri)
{
  HTTPClient http;
  http.begin(serviceUri);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    AUTOCONFIG_DBG(serviceUri << " error: " << httpCode);
    return INADDR_NONE;
  }

  String response = http.getString();
  AUTOCONFIG_DBG(serviceUri << " response: " << response);

  IPAddress ip;
  if (!WiFi.hostByName(response.c_str(), ip)) {
    AUTOCONFIG_DBG("DNS error");
    return INADDR_NONE;
  }

  AUTOCONFIG_DBG("DNS resolved to: " << ip);
  return ip;
}

} // namespace ndn