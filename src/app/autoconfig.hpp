#ifndef ESP8266NDN_APP_AUTOCONFIG_HPP
#define ESP8266NDN_APP_AUTOCONFIG_HPP

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include <IPAddress.h>
#include <WString.h>

namespace esp8266ndn {

/**
 * @brief Query NDN-FCH service to find a nearby NDN router.
 * @return Router IP address, or "0.0.0.0" on failure.
 */
IPAddress
queryFchService(String serviceUri = "http://ndn-fch.named-data.net/");

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#endif // ESP8266NDN_APP_AUTOCONFIG_HPP
