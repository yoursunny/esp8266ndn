#ifndef ESP8266NDN_BLE_IMPL_ESP32_HPP
#define ESP8266NDN_BLE_IMPL_ESP32_HPP

#include "ble-uuid.hpp"
#include <BLEDevice.h>
#include <WString.h>

#define BLEUUID_FROM_ARRAY(array) \
  BLEUUID(const_cast<uint8_t*>(array), sizeof(array), false)

namespace ndn {
namespace detail {

class BleClientImpl;

class BleDeviceImplClass
{
public:
  int
  init(const char* deviceName, bool enableServer, int nClients)
  {
    BLEDevice::init(deviceName);
    BLEDevice::setMTU(517);
    return 0;
  }

  String
  getAddr()
  {
    return String(BLEDevice::getAddress().toString().data()) + " (addr-type=public)";
  }

  int
  startScanConnect(BleClientImpl& client);

  BleClientImpl*
  getCurrentClient()
  {
    return m_client;
  }

private:
  static void
  scanCb(BLEScanResults results);

private:
  BleClientImpl* m_client = nullptr;
};

extern BleDeviceImplClass BleDeviceImpl;

class BleServiceImpl
{
public:
  int
  begin()
  {
    m_server = BLEDevice::createServer();
    m_svc = m_server->createService(BLEUUID_FROM_ARRAY(BLE_UUID_SVC));
    m_rx = m_svc->createCharacteristic(BLEUUID_FROM_ARRAY(BLE_UUID_RX),
           BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    m_tx = m_svc->createCharacteristic(BLEUUID_FROM_ARRAY(BLE_UUID_TX),
           BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    m_svc->start();
    return 0;
  }

  int
  advertise()
  {
    m_server->startAdvertising();
    return 0;
  }

  size_t
  receive(uint8_t* buf, size_t bufSize)
  {
    std::string value = m_rx->getValue();
    if (value.empty()) {
      return 0;
    }
    m_rx->setValue(buf, 0);

    if (value.size() > bufSize) {
      return 0;
    }
    memcpy(buf, value.data(), value.size());
    return value.size();
  }

  bool
  send(const uint8_t* pkt, size_t len)
  {
    m_tx->setValue(const_cast<uint8_t*>(pkt), len);
    m_tx->notify();
    return true;
  }

private:
  BLEServer* m_server = nullptr;
  BLEService* m_svc = nullptr;
  BLECharacteristic* m_rx = nullptr;
  BLECharacteristic* m_tx = nullptr;
};

class BleClientImpl
{
public:
  BleClientImpl()
    : state(State::IDLE)
    , addr("")
  {
  }

  bool
  begin()
  {
    return true;
  }

  size_t
  receive(uint8_t* buf, size_t bufSize)
  {
    switch (this->state) {
      case State::CONNECTED:
        break;
      case State::CONNECTING: {
        int err = this->connect();
        if (err == 0) {
          break;
        }
        // fallthrough
      }
      default:
        return 0;
    }

    uint16_t len = m_rxLen;
    memcpy(buf, m_rxData, std::min<uint16_t>(bufSize, len));
    m_rxLen = 0;
    return len;
  }

  bool
  send(const uint8_t* pkt, size_t len)
  {
    if (this->state != State::CONNECTED) {
      return false;
    }
    try {
      m_tx->writeValue(const_cast<uint8_t*>(pkt), len);
    }
    catch (const BLEDisconnectedException&) {
      this->state = State::DISCONNECTED;
      return false;
    }
    return true;
  }

private:
  int
  connect()
  {
    m_client = BLEDevice::createClient();
    m_client->connect(this->addr, this->addrType);
    m_svc = m_client->getService(BLEUUID_FROM_ARRAY(BLE_UUID_SVC));
    if (m_svc == nullptr) {
      m_client->disconnect();
      return __LINE__;
    }
    m_rx = m_svc->getCharacteristic(BLEUUID_FROM_ARRAY(BLE_UUID_TX));
    m_tx = m_svc->getCharacteristic(BLEUUID_FROM_ARRAY(BLE_UUID_RX));
    if (m_rx == nullptr || !m_rx->canRead() || !m_rx->canNotify() ||
        m_tx == nullptr || !m_tx->canWrite()) {
      m_client->disconnect();
      return __LINE__;
    }
    m_rx->registerForNotify(&BleClientImpl::rxNotify);
    this->state = State::CONNECTED;
    return 0;
  }

  static void
  rxNotify(BLERemoteCharacteristic* ch, uint8_t* data, size_t len, bool isNotify)
  {
    BleClientImpl* self = BleDeviceImpl.getCurrentClient();
    if (self == nullptr || self->m_rx != ch) {
      return;
    }
    memcpy(self->m_rxData, data, len);
    self->m_rxLen = len;
  }

public:
  enum class State {
    UNINITIALIZED,
    IDLE,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
  };
  State state;
  BLEAddress addr;
  esp_ble_addr_type_t addrType;

private:
  BLEClient* m_client = nullptr;
  BLERemoteService* m_svc = nullptr;
  BLERemoteCharacteristic* m_rx = nullptr;
  BLERemoteCharacteristic* m_tx = nullptr;
  uint16_t m_rxLen = 0;
  uint8_t m_rxData[517];
};

inline int
BleDeviceImplClass::startScanConnect(BleClientImpl& client)
{
  if (m_client != nullptr) {
    return __LINE__; // only support one client
  }
  m_client = &client;

  BLEDevice::getScan()->setActiveScan(false);
  BLEDevice::getScan()->start(10, &BleDeviceImplClass::scanCb, false);
  return 0;
}

inline void
BleDeviceImplClass::scanCb(BLEScanResults results)
{
  bool hasPeer = false;
  BLEAddress addr("");
  esp_ble_addr_type_t addrType;

  BLEUUID uuidSvc = BLEUUID_FROM_ARRAY(BLE_UUID_SVC);
  for (int i = 0, count = results.getCount(); i < count; ++i) {
    BLEAdvertisedDevice peer = results.getDevice(i);
    if (peer.isAdvertisingService(uuidSvc)) {
      hasPeer = true;
      addr = peer.getAddress();
      addrType = peer.getAddressType();
      break;
    }
  }
  BLEDevice::getScan()->clearResults();

  BleClientImpl* client = BleDeviceImpl.m_client;
  if (hasPeer && client != nullptr) {
    client->state = BleClientImpl::State::CONNECTING;
    client->addr = addr;
    client->addrType = addrType;
  }
}

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_BLE_IMPL_ESP32_HPP
