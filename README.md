# NDN Arduino library for ESP8266 microcontroller

**esp8266ndn** library enables [Named Data Networking](https://named-data.net/) application development on [ESP8266 Arduino core](https://github.com/esp8266/Arduino) and [ESP32 Arduino core](https://github.com/espressif/arduino-esp32).

The following features are implemented in this library:

* Encode and decode NDN Interest/Data/Nack packets, using [ndn-cpp-lite](https://github.com/named-data/ndn-cpp)
* Connect to a remote NDN forwarder over unicast UDP tunnel
* Communicate with neighbors via Ethernet or UDP multicast
* Communicate with PyNDN application via Bluetooth Low Energy
* HMAC signing and verification, using [Cryptosuite](https://github.com/Cathedrow/Cryptosuite)
* ECDSA signing and verification, using [micro-ecc](https://github.com/kmackay/micro-ecc)

This is a side project owned by [yoursunny.com](https://yoursunny.com). It is not an official part of the NDN platform. The author provides best-effort support on [ndn-lib mailing list](https://www.lists.cs.ucla.edu/mailman/listinfo/ndn-lib).

## Installation

Clone this repository under `$HOME/Arduino/libraries` directory.
Add `#include <esp8266ndn.h>` to your sketch.
Check out the [examples](examples/) for how to use.
