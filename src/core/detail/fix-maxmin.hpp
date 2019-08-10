#ifdef ARDUINO_ARCH_NRF52
#undef max
#undef min
#undef round
#endif // ARDUINO_ARCH_NRF52

#ifndef ESP8266NDN_FIX_MAXMIN_HPP
#define ESP8266NDN_FIX_MAXMIN_HPP

#ifdef ARDUINO_ARCH_NRF52

#include <algorithm>
#include <cmath>

using std::max;
using std::min;
using std::round;

#endif // ARDUINO_ARCH_NRF52

#endif // ESP8266NDN_FIX_MAXMIN_HPP
