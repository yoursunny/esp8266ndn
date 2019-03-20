#ifndef ESP8266NDN_BLE_IMPL_HPP
#define ESP8266NDN_BLE_IMPL_HPP

#if defined(ESP32)
#include "ble-impl-esp32.hpp"
#elif defined(ARDUINO_ARCH_NRF52)
#include "ble-impl-bluefruit.hpp"
#else
#include "ble-impl-null.hpp"
#endif

#endif // ESP8266NDN_BLE_IMPL_HPP
