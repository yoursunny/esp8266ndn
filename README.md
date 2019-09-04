# NDN Arduino library for ESP8266 and more

**esp8266ndn** library enables [Named Data Networking](https://named-data.net/) application development in Arduino environment. It supports [ESP8266](https://github.com/esp8266/Arduino), [ESP32](https://github.com/espressif/arduino-esp32), and [Adafruit nRF52](https://github.com/adafruit/Adafruit_nRF52_Arduino) microcontrollers.

* [Doxygen documentation](https://esp8266ndn.netlify.com/)
* [#esp8266ndn on Twitter](https://twitter.com/hashtag/esp8266ndn) for announcements
* [ndn-lib mailing list](https://www.lists.cs.ucla.edu/mailman/listinfo/ndn-lib) for best-effort support

## Features

Packet encoding and decoding

* Interest, Data, and Nack
  * Decoding recognizes both [v0.2](https://named-data.net/doc/NDN-packet-spec/0.2/) and [v0.3](https://named-data.net/doc/NDN-packet-spec/0.3/) formats
  * Interest encoding defaults to v0.3 format
* Signed Interest: [2014 format](https://redmine.named-data.net/projects/ndn-cxx/wiki/SignedInterest)
* Naming Convention: [2014 format](https://named-data.net/publications/techreports/ndn-tr-22-ndn-memo-naming-conventions/)

Transports

* Ethernet: unicast and multicast
  * ESP8266: yes, requires lwIP 1.4 (in Arduino *Tools* menu select "lwIP Variant: v1.4 Higher Bandwidth")
  * ESP32: yes
* UDP: unicast and multicast
  * ESP8266: yes
  * ESP32: yes
* Bluetooth Low Energy
  * ESP32: server/peripheral and client/central
  * nRF52: server/peripheral only
* LoRa radio
  * ESP32: yes, only tested with Heltec WiFi\_LoRa\_32 board

Crypto

* SHA256 and HMAC-SHA256
  * ESP8266: yes, using BearSSL from Arduino Core
  * ESP32: yes, using mbed TLS from ESP-IDF
  * nRF52: yes, using CryptoSuite
* ECDSA
  * ESP8266: yes, using micro-ecc 'static'
  * ESP32: yes, using mbed TLS from ESP-IDF
  * nRF52: insecure, lacks random number generator integration

Forwarding

* ndn-lite forwarder
  * experimental, only tested with ESP8266

Application layer services

* [ndnping](https://github.com/named-data/ndn-tools/tree/master/tools/ping) server and client
* [NDN-FCH](https://github.com/named-data/NDN-FCH) client for connecting to the global NDN testbed
  * ESP8266 and ESP32 only
* UnixTime client for time synchronization

## Installation

Clone this repository under `$HOME/Arduino/libraries` directory.
Add `#include <esp8266ndn.h>` to your sketch.
Check out the [examples](examples/) for how to use.
