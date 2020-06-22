#ifndef ESP8266NDN_PORT_PORT_HPP
#define ESP8266NDN_PORT_PORT_HPP

#include "choose.hpp"

#ifdef ESP8266NDN_PORT_SHA256_BEARSSL
#include "sha256-bearssl.hpp"
#endif

#ifdef ESP8266NDN_PORT_SHA256_CRYPTOSUITE
#include "sha256-cryptosuite.hpp"
#endif

#ifdef ESP8266NDN_PORT_EC_UECC
#include "ec-uecc.hpp"
#endif

#ifdef ESP8266NDN_PORT_QUEUE_FREERTOS
#include "queue-freertos.hpp"
#endif

#define NDNPH_PORT_RANDOM_CUSTOM
#include "random.hpp"

#include <NDNph.h>

#endif // ESP8266NDN_PORT_PORT_HPP
