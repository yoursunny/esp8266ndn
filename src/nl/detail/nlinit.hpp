#ifndef ESP8266NDN_NLINIT_HPP
#define ESP8266NDN_NLINIT_HPP

namespace ndn {
namespace detail {

void
nlInitSecurity();

void
nlInitKeyStorage();

void
nlInitForwarder();

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_NLINIT_HPP
