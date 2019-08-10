#ifndef ESP8266NDN_EC_IMPL_HPP
#define ESP8266NDN_EC_IMPL_HPP

#if defined(ESP32)
#include "ec-impl-mbedtls.hpp"
#else
#include "ec-impl-microecc.hpp"
#endif

#if defined(ESP32)
#include "eckeygen-impl-mbedtls.hpp"
#else
#include "eckeygen-impl-microecc.hpp"
#endif

#endif // ESP8266NDN_EC_IMPL_HPP
