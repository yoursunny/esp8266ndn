#ifndef ESP8266NDN_ESP32_1734_HPP
#define ESP8266NDN_ESP32_1734_HPP

// https://github.com/espressif/arduino-esp32/issues/1734 workaround
#undef min
#undef max
#undef round
#include <algorithm>
#include <cmath>
using std::isinf;
using std::isnan;
using std::max;
using std::min;
using ::round;

#endif // ESP8266NDN_ESP32_1734_HPP