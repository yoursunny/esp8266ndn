#ifndef ESP8266NDN_BLE_IMPL_ESP32_HPP
#define ESP8266NDN_BLE_IMPL_ESP32_HPP

#include "ble-uuid.hpp"
#include <BLEDevice.h>
#include <WString.h>

#define BLEUUID_FROM_ARRAY(array) \
  BLEUUID(const_cast<uint8_t*>(array), sizeof(array), false)

namespace ndn {

class BleServiceImpl;

class BleDeviceImplClass
{
public:
  int
  init(const char* deviceName, bool enableServer, int nClients)
  {
    BLEDevice::init(deviceName);
    BLEDevice::setMTU(517);

    if (enableServer) {
      this->server = BLEDevice::createServer();
    }

    return 0;
  }

  String
  getAddr()
  {
    return String(BLEDevice::getAddress().toString().data()) + " (addr-type=public)";
  }

  int
  advertiseService(BleServiceImpl* service)
  {
    this->server->startAdvertising();
  }

public:
  BLEServer* server = nullptr;
};

extern BleDeviceImplClass BleDeviceImpl;

class BleServiceImpl
{
public:
  BleServiceImpl()
    : m_svc(nullptr)
    , m_rx(nullptr)
    , m_tx(nullptr)
  {
  }

  int
  begin()
  {
    m_svc = BleDeviceImpl.server->createService(BLEUUID_FROM_ARRAY(BLE_UUID_SVC));
    m_rx = m_svc->createCharacteristic(BLEUUID_FROM_ARRAY(BLE_UUID_RX),
           BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    m_tx = m_svc->createCharacteristic(BLEUUID_FROM_ARRAY(BLE_UUID_TX),
           BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    m_svc->start();
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

  void
  send(const uint8_t* pkt, size_t len)
  {
    m_tx->setValue(const_cast<uint8_t*>(pkt), len);
    m_tx->notify();
  }

private:
  BLEService* m_svc;
  BLECharacteristic* m_rx;
  BLECharacteristic* m_tx;
};

} // namespace ndn

#endif // ESP8266NDN_BLE_IMPL_ESP32_HPP
