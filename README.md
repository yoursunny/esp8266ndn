# NDN Arduino library for ESP8266 and more

[![Travis build status](https://img.shields.io/travis/com/yoursunny/esp8266ndn?style=flat)](https://travis-ci.com/github/yoursunny/esp8266ndn) [![GitHub code size](https://img.shields.io/github/languages/code-size/yoursunny/esp8266ndn?style=flat)](https://github.com/yoursunny/esp8266ndn)

**esp8266ndn** library enables [Named Data Networking](https://named-data.net/) application development in Arduino environment. It supports [ESP8266](https://github.com/esp8266/Arduino), [ESP32](https://github.com/espressif/arduino-esp32), and [Adafruit nRF52](https://github.com/adafruit/Adafruit_nRF52_Arduino) microcontrollers.

* [Doxygen documentation](https://esp8266ndn.ndn.today/)
* [#esp8266ndn on Twitter](https://twitter.com/hashtag/esp8266ndn) for announcements
* [ndn-lib mailing list](https://www.lists.cs.ucla.edu/mailman/listinfo/ndn-lib) for best-effort support

![esp8266ndn logo](docs/logo.svg)

## Features

Packet encoding and decoding

* Interest and Data
  * [v0.3](https://named-data.net/doc/NDN-packet-spec/0.3/) format only
  * TLV evolvability: yes
  * Forwarding Hint: no
* [NDNLPv2](https://redmine.named-data.net/projects/nfd/wiki/NDNLPv2)
  * fragmentation and reassembly: no
  * Nack: yes
  * PIT token: yes
  * congestion mark: no
  * link layer reliability: no
* Signed Interest: [v0.3 format](https://named-data.net/doc/NDN-packet-spec/0.3/signed-interest.html)
* Naming Convention: [2019 format](https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/)

Transports

* Ethernet: unicast and multicast
  * ESP8266: yes, requires lwIP 1.4 (in Arduino *Tools* menu select "lwIP Variant: v1.4 Higher Bandwidth")
  * ESP32: yes
* UDP: unicast and multicast
  * ESP8266: yes
  * ESP32: yes
* Bluetooth Low Energy
  * ESP32: server/peripheral only
  * nRF52: server/peripheral only

KeyChain - Crypto

* SHA256
  * ESP8266: yes, using BearSSL from Arduino Core
  * ESP32: yes, using mbed TLS from ESP-IDF
  * nRF52: yes, using CryptoSuite
* ECDSA: P-256 curve only
  * ESP8266 and nRF52: yes, using micro-ecc
  * ESP32: yes, using mbed TLS from ESP-IDF
* HMAC-SHA256: no
* RSA: no
* Null: yes

KeyChain - Services

* [NDN certificates](https://named-data.net/doc/ndn-cxx/0.7.0/specs/certificate-format.html): basic support
* NDNCERT protocol: no
* Key storage: no
* Trust schema: no

Application layer services

* [ndnping](https://github.com/named-data/ndn-tools/tree/master/tools/ping) server and client
* [NDN-FCH](https://github.com/named-data/NDN-FCH) client for connecting to the global NDN testbed
  * ESP8266 and ESP32 only
* UnixTime client for time synchronization

## Installation

1. Clone [NDNph](https://github.com/yoursunny/NDNph) and this repository under `$HOME/Arduino/libraries` directory.
2. Add `#include <esp8266ndn.h>` to your sketch.
3. Check out the [examples](examples/) for how to use.
