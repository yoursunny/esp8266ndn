#ifndef ESP8266NDN_PORT_PORT_HPP
#define ESP8266NDN_PORT_PORT_HPP

#if defined(ARDUINO_ARCH_ESP32)
#define NDNPH_PORT_CRYPTO_MBEDTLS
#endif

#if defined(ARDUINO_ARCH_ESP32)
#define NDNPH_PORT_QUEUE_CUSTOM
#include "esp32-queue.hpp"
#else
#define NDNPH_PORT_QUEUE_SIMPLE
#endif

#define NDNPH_PORT_RANDOM_CUSTOM
#include "random.hpp"

#include <NDNph.h>

#endif // ESP8266NDN_PORT_PORT_HPP
