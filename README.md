# NDN Arduino library for ESP8266 and more

**esp8266ndn** library enables [Named Data Networking](https://named-data.net/) application development in Arduino environment. It supports [ESP8266](https://github.com/esp8266/Arduino), [ESP32](https://github.com/espressif/arduino-esp32), and [Adafruit nRF52](https://github.com/adafruit/Adafruit_nRF52_Arduino) microcontrollers.

The following features are implemented in this library:

* Encode and decode NDN Interest/Data/Nack packets
* Communicate via Ethernet unicast and Ethernet multicast (ESP8266 and ESP32)
* Communicate via UDP unicast and UDP multicast (ESP8266 and ESP32)
* Communicate via Bluetooth Low Energy (ESP32 and nRF52)
* Communicate via LoRa radio (ESP32 only)
* SHA256 signing and verification
* HMAC-SHA256 signing and verification
* ECDSA signing and verification, and EC key generation
* [ndnping](https://github.com/named-data/ndn-tools/tree/master/tools/ping) server and client
* [NDN-FCH](https://github.com/named-data/NDN-FCH) client for connecting to the global NDN testbed (ESP8266 and ESP32)

This is a side project owned by [yoursunny.com](https://yoursunny.com). It is not an official part of the NDN platform. The author provides best-effort support on [ndn-lib mailing list](https://www.lists.cs.ucla.edu/mailman/listinfo/ndn-lib).

## Installation

Clone this repository under `$HOME/Arduino/libraries` directory.
Add `#include <esp8266ndn.h>` to your sketch.
Check out the [examples](examples/) for how to use.
