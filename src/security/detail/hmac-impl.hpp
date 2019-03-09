#ifndef ESP8266NDN_HMAC_IMPL_HPP
#define ESP8266NDN_HMAC_IMPL_HPP

#if defined(ESP8266)
#include "hmac-impl-bearssl.hpp"
#elif defined(ESP32)
#include "hmac-impl-mbedtls.hpp"
#else
#include "hmac-impl-none.hpp"
#endif

#endif // ESP8266NDN_HMAC_IMPL_HPP
