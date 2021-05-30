#ifndef ESP8266NDN_APP_AUTOCONFIG_HPP
#define ESP8266NDN_APP_AUTOCONFIG_HPP

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include <IPAddress.h>
#include <WString.h>

class WiFiClient;

namespace esp8266ndn {

struct FchResponse
{
  bool ok = false;
  IPAddress ip;
};

/**
 * @brief Query NDN-FCH service to find a nearby NDN router.
 * @param client @c WiFiClient or @c WiFiClientSecure instance.
 * @param serviceUri NDN-FCH service base URI.
 */
FchResponse
fchQuery(::WiFiClient& client, String serviceUri = "https://fch.ndn.today/");

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#endif // ESP8266NDN_APP_AUTOCONFIG_HPP
