#ifndef ESP8266NDN_APP_AUTOCONFIG_HPP
#define ESP8266NDN_APP_AUTOCONFIG_HPP

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)

#include <IPAddress.h>
#include <WString.h>

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_RP2040)
#define ESP8266NDN_Network WiFi
#define ESP8266NDN_NetworkClient WiFiClient
#elif defined(ARDUINO_ARCH_ESP32)
#define ESP8266NDN_Network Network
#define ESP8266NDN_NetworkClient NetworkClient
#endif

class ESP8266NDN_NetworkClient;

#if defined(ARDUINO_ARCH_RP2040)
using arduino::IPAddress;
using arduino::String;
#endif

namespace esp8266ndn {

struct FchResponse {
  bool ok = false;
  IPAddress ip;
};

/**
 * @brief Query NDN-FCH service to find a nearby NDN router.
 * @param client @c WiFiClient or @c WiFiClientSecure instance on ESP8266;
 *               @c NetworkClient or @c NetworkClientSecure instance on ESP32.
 * @param serviceUri NDN-FCH service base URI.
 */
FchResponse
fchQuery(ESP8266NDN_NetworkClient& client, String serviceUri = "https://fch.ndn.today/");

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#endif // ESP8266NDN_APP_AUTOCONFIG_HPP
