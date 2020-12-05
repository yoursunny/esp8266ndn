# NDN Arduino library for ESP8266 and more

[![GitHub Workflow status](https://img.shields.io/github/workflow/status/yoursunny/esp8266ndn/build?style=flat)](https://github.com/yoursunny/esp8266ndn/actions) [![GitHub code size](https://img.shields.io/github/languages/code-size/yoursunny/esp8266ndn?style=flat)](https://github.com/yoursunny/esp8266ndn)

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
  * forwarding hint: no
* [NDNLPv2](https://redmine.named-data.net/projects/nfd/wiki/NDNLPv2)
  * fragmentation and reassembly: no
  * Nack: partial
  * PIT token: yes
  * congestion mark: no
  * link layer reliability: no
* Signed Interest: [v0.3 format](https://named-data.net/doc/NDN-packet-spec/0.3/signed-interest.html)
* Naming Convention: [2019 format](https://named-data.net/publications/techreports/ndn-tr-22-2-ndn-memo-naming-conventions/)

Transports

* Ethernet: unicast and multicast on ESP8266 and ESP32
  * ESP8266: requires lwIP 1.4 (in Arduino *Tools* menu select "lwIP Variant: v1.4 Higher Bandwidth")
* UDP: unicast and multicast on ESP8266 and ESP32
* Bluetooth Low Energy: server/peripheral only on ESP32 and nRF52
  * Compatible with [NDNts](https://yoursunny.com/p/NDNts/) `@ndn/web-bluetooth-transport` package

KeyChain

* Crypto
  * SHA256: yes (using BearSSL on ESP8266, Mbed TLS on ESP32, CryptoSuite on nRF52)
  * ECDSA: P-256 curve only (using Mbed TLS on ESP32, micro-ecc on ESP8266 and nRF52)
  * HMAC-SHA256: no
  * RSA: no
  * Null: yes
* [NDN certificates](https://named-data.net/doc/ndn-cxx/0.7.1/specs/certificate-format.html): basic support
* Persistent key and certificate storage: binary files
* Trust schema: no

Application layer services

* [ndnping](https://github.com/named-data/ndn-tools/tree/master/tools/ping) server and client
* segmented object producer and consumer
* [Realtime Data Retrieval (RDR)](https://redmine.named-data.net/projects/ndn-tlv/wiki/RDR) metadata producer
* [NDNCERT](https://github.com/named-data/ndncert/wiki/NDNCERT-Protocol-0.3) (work in progress)
* [NDN-FCH](https://github.com/named-data/NDN-FCH) client for connecting to the global NDN testbed
  * ESP8266 and ESP32 only
* [UnixTime](https://github.com/yoursunny/ndn6-tools/blob/master/unix-time-service.md) client for time synchronization

## Installation

1. Clone [NDNph](https://github.com/yoursunny/NDNph) and this repository under `$HOME/Arduino/libraries` directory.
2. Add `#include <esp8266ndn.h>` to your sketch.
3. Check out the [examples](examples/) for how to use.
