#ifndef ESP8266NDN_BLE_SERVER_TRANSPORT_HPP
#define ESP8266NDN_BLE_SERVER_TRANSPORT_HPP

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_NRF52)

#include "../port/port.hpp"

#if defined(ARDUINO_ARCH_ESP32)
#include <BLEDevice.h>
#elif defined(ARDUINO_ARCH_NRF52)
#include <bluefruit.h>
#endif

namespace esp8266ndn {

/** @brief A transport that acts as a BLE server/peripheral. */
class BleServerTransport
  : public virtual ndnph::Transport
  , public ndnph::transport::DynamicRxQueueMixin
#if defined(ARDUINO_ARCH_NRF52)
  , public ::BLEService
#endif
{
public:
  BleServerTransport();

  /** @brief Initialize BLE device, service, and advertisement. */
  bool begin(const char* deviceName);

#if defined(ARDUINO_ARCH_NRF52)
  /** @brief Initialize BLE service only. */
  err_t begin() final;
#endif

  String getAddr() const;

private:
  bool doIsUp() const final;

  void doLoop() final;

  bool doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId) final;

  void receiveImpl(const uint8_t* pkt, size_t pktLen, uint64_t endpointId);

#if defined(ARDUINO_ARCH_ESP32)
  class CsCallbacks : public BLECharacteristicCallbacks
  {
  public:
    explicit CsCallbacks(BleServerTransport& transport);

    void onWrite(::BLECharacteristic* chr) final;

  private:
    BleServerTransport& m_transport;
  };
  CsCallbacks m_csCallbackHandler;

  ::BLEServer* m_server = nullptr;
  ::BLEService* m_svc = nullptr;
  ::BLECharacteristic* m_cs = nullptr;
  ::BLECharacteristic* m_sc = nullptr;
#elif defined(ARDUINO_ARCH_NRF52)
  static void handleCsWrite(uint16_t connHdl, ::BLECharacteristic* chr, uint8_t* pkt,
                            uint16_t pktLen);

  ::BLECharacteristic m_cs;
  ::BLECharacteristic m_sc;
#endif
};

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_NRF52)

#endif // ESP8266NDN_BLE_SERVER_TRANSPORT_HPP
