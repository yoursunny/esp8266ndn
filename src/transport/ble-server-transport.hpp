#ifndef ESP8266NDN_BLE_SERVER_TRANSPORT_HPP
#define ESP8266NDN_BLE_SERVER_TRANSPORT_HPP

#include "../port/port.hpp"
#include "ble-uuid.hpp"

#if defined(ARDUINO_ARCH_ESP32) && !defined(CONFIG_BT_NIMBLE_ENABLED)
#include <BLEDevice.h>
#elif defined(ARDUINO_ARCH_NRF52)
#include <bluefruit.h>
#endif

namespace esp8266ndn {

class BleServerTransportBase
  : public virtual ndnph::Transport
  , public ndnph::transport::DynamicRxQueueMixin {
protected:
  explicit BleServerTransportBase(size_t mtu);

  void handleReceive(const uint8_t* pkt, size_t pktLen, uint64_t endpointId);

private:
  void doLoop() override;
};

#if defined(ARDUINO_ARCH_ESP32) &&                                                                 \
  (defined(CONFIG_BT_NIMBLE_ENABLED) || defined(CONFIG_BT_BLUEDROID_ENABLED))
#define ESP8266NDN_HAVE_ESP32BLE

/** @brief A transport that acts as a BLE server/peripheral. */
class BleServerTransport : public BleServerTransportBase {
public:
  static size_t getMtu() {
    return 512;
  }

  BleServerTransport()
    : BleServerTransportBase(getMtu())
    , m_csCallbackHandler(*this) {}

  /** @brief Initialize BLE device, service, and advertisement. */
  bool begin(const char* deviceName) {
    ::BLEDevice::init(deviceName);
    ::BLEDevice::setMTU(517);

#ifdef CONFIG_BT_NIMBLE_ENABLED
#define BLEPROP(prop) NIMBLE_PROPERTY::prop
#else
#define BLEPROP(prop) ::BLECharacteristic::PROPERTY_##prop
    warnBluedroid();
#endif

    m_server = ::BLEDevice::createServer();
    m_svc = m_server->createService(makeUuid(BLE_UUID_SVC));
    m_cs = m_svc->createCharacteristic(makeUuid(BLE_UUID_CS), BLEPROP(WRITE) | BLEPROP(NOTIFY));
    m_cs->setCallbacks(&m_csCallbackHandler);
    m_sc = m_svc->createCharacteristic(makeUuid(BLE_UUID_SC), BLEPROP(READ) | BLEPROP(NOTIFY));
    m_svc->start();

#undef BLEPROP

    auto adv = ::BLEDevice::getAdvertising();
    adv->addServiceUUID(makeUuid(BLE_UUID_SVC));
    adv->setScanResponse(true);
    adv->start();
    return true;
  }

  /** @brief Retrieve BLE address and address type. */
  String getAddr() const {
    return String(::BLEDevice::getAddress().toString().c_str()) + " (addr-type=public)";
  }

private:
  static ::BLEUUID makeUuid(const uint8_t a[16]) {
    return ::BLEUUID(const_cast<uint8_t*>(a), 16, false);
  }

  static void warnBluedroid();

  bool doIsUp() const final {
    return m_server != nullptr && m_server->getConnectedCount() > 0;
  }

  bool doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId) final {
    if (m_sc == nullptr) {
      return false;
    }
    m_sc->setValue(const_cast<uint8_t*>(pkt), pktLen);
    m_sc->notify();
    return true;
  }

private:
  class CsCallbacks : public ::BLECharacteristicCallbacks {
  public:
    explicit CsCallbacks(BleServerTransport& transport)
      : m_transport(transport) {}

    void onWrite(::BLECharacteristic* chr) final {
      if (chr != m_transport.m_cs) {
        return;
      }
      auto value = chr->getValue();
      m_transport.handleReceive(reinterpret_cast<const uint8_t*>(value.c_str()), value.length(), 0);
    }

  private:
    BleServerTransport& m_transport;
  };
  CsCallbacks m_csCallbackHandler;

  ::BLEServer* m_server = nullptr;
  ::BLEService* m_svc = nullptr;
  ::BLECharacteristic* m_cs = nullptr;
  ::BLECharacteristic* m_sc = nullptr;
};

#elif defined(ARDUINO_ARCH_NRF52)

/**
 * @brief A transport that acts as a BLE server/peripheral.
 *
 * @bug Rapidly transmitting multiple packets can cause significant packet loss due to lack of
 *      queuing. Such loss severely affects fragmentation.
 */
class BleServerTransport
  : public BleServerTransportBase
  , public ::BLEService {
public:
  static size_t getMtu() {
    return BLE_GATT_ATT_MTU_MAX - 3;
  }

  BleServerTransport()
    : BleServerTransportBase(getMtu())
    , ::BLEService(BLE_UUID_SVC)
    , m_cs(BLE_UUID_CS)
    , m_sc(BLE_UUID_SC) {}

  /** @brief Initialize BLE device, service, and advertisement. */
  bool begin(const char* deviceName) {
    Bluefruit.configPrphConn(BLE_GATT_ATT_MTU_MAX, BLE_GAP_EVENT_LENGTH_DEFAULT,
                             BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT,
                             BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT);
    VERIFY(Bluefruit.begin(1, 0));
    Bluefruit.setName(deviceName);
    Bluefruit.setTxPower(4);

    VERIFY_STATUS(begin(), false);

    VERIFY(Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE));
    VERIFY(Bluefruit.Advertising.addTxPower());
    VERIFY(Bluefruit.Advertising.addService(*this));
    VERIFY(Bluefruit.ScanResponse.addName());
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244);
    Bluefruit.Advertising.setFastTimeout(30);
    VERIFY(Bluefruit.Advertising.start());

    return true;
  }

  /** @brief Initialize BLE service only. */
  err_t begin() final {
    VERIFY_STATUS(this->BLEService::begin());

    uint16_t mtu = Bluefruit.getMaxMtu(CONN_CFG_PERIPHERAL);
    m_cs.setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
    m_cs.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    m_cs.setMaxLen(mtu);
    m_cs.setWriteCallback(BleServerTransport::handleCsWrite);
    VERIFY_STATUS(m_cs.begin());

    m_sc.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
    m_sc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    m_sc.setMaxLen(mtu);
    VERIFY_STATUS(m_sc.begin());

    return 0;
  }

  /** @brief Retrieve BLE address and address type. */
  String getAddr() const {
    uint8_t mac[6]; // reversed
    uint8_t addrType = Bluefruit.getAddr(mac);
    char addr[18];
    snprintf(addr, sizeof(addr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2],
             mac[1], mac[0]);
    if (addrType == BLE_GAP_ADDR_TYPE_RANDOM_STATIC) {
      return String(addr) + " (addr-type=random-static)";
    }
    return String(addr) + " (addr-type=" + static_cast<int>(addrType) + ")";
  }

private:
  bool doIsUp() const final {
    return Bluefruit.connected() > 0;
  }

  bool doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId) final {
    m_sc.write(pkt, pktLen);
    return m_sc.notify(pkt, pktLen);
  }

  static void handleCsWrite(uint16_t connHdl, ::BLECharacteristic* chr, uint8_t* pkt,
                            uint16_t pktLen) {
    BleServerTransport& self = static_cast<BleServerTransport&>(chr->parentService());
    self.handleReceive(pkt, pktLen, connHdl);
  }

  ::BLECharacteristic m_cs;
  ::BLECharacteristic m_sc;
};

#endif // ARDUINO_ARCH_*

} // namespace esp8266ndn

#endif // ESP8266NDN_BLE_SERVER_TRANSPORT_HPP
