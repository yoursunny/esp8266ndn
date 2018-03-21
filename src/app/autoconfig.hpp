#ifndef ESP8266NDN_AUTOCONFIG_HPP
#define ESP8266NDN_AUTOCONFIG_HPP

#include <IPAddress.h>
#include <WString.h>

namespace ndn {

/** \brief Query NDN-FCH service to find a nearby NDN router.
 *  \return Router IP address, or INADDR_NONE on failure.
 */
IPAddress
queryFchService(String serviceUri = "http://ndn-fch.named-data.net/");

} // namespace ndn

#endif // ESP8266NDN_AUTOCONFIG_HPP