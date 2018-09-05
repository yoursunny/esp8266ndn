#ifndef ESP8266NDN_EC_IMPL_HPP
#define ESP8266NDN_EC_IMPL_HPP

#if defined(ESP8266)
#include "ec-impl-bearssl.hpp"
#else
#include "ec-impl-microecc.hpp"
#endif

#endif // ESP8266NDN_EC_IMPL_HPP
