#ifndef ESP8266NDN_HMAC_IMPL_HPP
#define ESP8266NDN_HMAC_IMPL_HPP

#if defined(ESP8266)
#include "sha256-impl-bearssl.hpp"
#elif defined(ESP32)
#include "sha256-impl-mbedtls.hpp"
#else
#include "sha256-impl-none.hpp"
#endif

#endif // ESP8266NDN_HMAC_IMPL_HPP
