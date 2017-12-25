#ifndef ESP8266NDN_LOGGING_HPP
#define ESP8266NDN_LOGGING_HPP

#include <Print.h>

namespace ndn {

Print&
getLogOutput();

void
setLogOutput(Print& output);

} // namespace ndn

#endif // ESP8266NDN_LOGGING_HPP
