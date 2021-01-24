#ifndef ESP8266NDN_PORT_CHOOSE_HPP
#define ESP8266NDN_PORT_CHOOSE_HPP

#if defined(ARDUINO_ARCH_ESP8266)

#define NDNPH_PORT_SHA256_CUSTOM
#define ESP8266NDN_PORT_SHA256_BEARSSL

#define NDNPH_PORT_EC_CUSTOM
#define ESP8266NDN_PORT_EC_UECC

#define NDNPH_PORT_QUEUE_SIMPLE

#define NDNPH_PORT_UNIXTIME_SYSTIME
#define NDNPH_PORT_UNIXTIME_SYSTIME_CANSET

#elif defined(ARDUINO_ARCH_ESP32)

#define NDNPH_HAVE_MBED

#define NDNPH_PORT_QUEUE_CUSTOM
#define ESP8266NDN_PORT_QUEUE_FREERTOS

#define NDNPH_PORT_UNIXTIME_SYSTIME
#define NDNPH_PORT_UNIXTIME_SYSTIME_CANSET

#elif defined(ARDUINO_ARCH_NRF52)

#define NDNPH_PORT_SHA256_CUSTOM
#define ESP8266NDN_PORT_SHA256_CRYPTOSUITE

#define NDNPH_PORT_EC_CUSTOM
#define ESP8266NDN_PORT_EC_UECC

#define NDNPH_PORT_QUEUE_SIMPLE

#else

#error "Unknown ARDUINO_ARCH"

#endif

#endif // ESP8266NDN_PORT_CHOOSE_HPP
