#include "autoconfig.hpp"
#include "../core/logger.hpp"

#if defined(ESP8266)
#define HAVE_HTTPCLIENT
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#define HAVE_HTTPCLIENT
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#define LOG(...) LOGGER(AutoConfig, __VA_ARGS__)

namespace ndn {

IPAddress
queryFchService(String serviceUri)
{
#ifdef HAVE_HTTPCLIENT
  WiFiClient tcp;
  HTTPClient http;
  http.begin(tcp, serviceUri);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    LOG(serviceUri << " error: " << httpCode);
    return INADDR_NONE;
  }
  String response = http.getString();
  LOG(serviceUri << " response: " << response);

  IPAddress ip;
  if (!WiFi.hostByName(response.c_str(), ip)) {
    LOG("DNS error");
    return INADDR_NONE;
  }
  LOG("DNS resolved to: " << ip);
  return ip;
#else // HAVE_HTTPCLIENT
  return INADDR_NONE;
#endif // HAVE_HTTPCLIENT
}

} // namespace ndn