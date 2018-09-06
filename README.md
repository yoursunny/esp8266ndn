# NDN Arduino library for ESP8266 and ESP32

**esp8266ndn** library enables [Named Data Networking](https://named-data.net/) application development on [ESP8266 Arduino core](https://github.com/esp8266/Arduino) and [ESP32 Arduino core](https://github.com/espressif/arduino-esp32).

The following features are implemented in this library:

* Encode and decode NDN Interest/Data/Nack packets
* Communicate via Ethernet unicast and Ethernet multicast
* Communicate via UDP unicast and UDP multicast
* Communicate via Bluetooth Low Energy and LoRa radio (ESP32 only)
* SHA256 signing and verification
* HMAC-SHA256 signing and verification
* ECDSA signing and verification, and EC key generation
* [ndnping](https://github.com/named-data/ndn-tools/tree/master/tools/ping) server and client
* [NDN-FCH](https://github.com/named-data/NDN-FCH) client for connecting to the global NDN testbed

This is a side project owned by [yoursunny.com](https://yoursunny.com). It is not an official part of the NDN platform. The author provides best-effort support on [ndn-lib mailing list](https://www.lists.cs.ucla.edu/mailman/listinfo/ndn-lib).

## Installation

Clone this repository under `$HOME/Arduino/libraries` directory.
Add `#include <esp8266ndn.h>` to your sketch.
Check out the [examples](examples/) for how to use.
