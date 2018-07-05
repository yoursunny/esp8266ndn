#ifdef ESP32
// Due to conflicting macro declaration in Arduino.h, BLEDevice.h must be included first.
#include <BLEDevice.h>
#include "ble-server-transport.hpp"
#include "logger.hpp"

#define BLESERVERTRANSPORT_DBG(...) DBG(BleServerTransport, __VA_ARGS__)

#define UUID_SVC "099577e3-0788-412a-8824-395084d97391"
#define UUID_RX  "cc5abb89-a541-46d8-a351-2f95a6a81f49"
#define UUID_TX  "972f9527-0d83-4261-b95d-b1b2fc73bde4"

namespace ndn {

BleServerTransport::BleServerTransport()
  : m_server(nullptr)
  , m_rx(nullptr)
  , m_tx(nullptr)
{
}

void
BleServerTransport::begin(const char* deviceName)
{
  BLEDevice::init(deviceName);
  BLEDevice::setMTU(517);
  m_server = BLEDevice::createServer();
  BLEService* service = m_server->createService(UUID_SVC);
  m_rx = service->createCharacteristic(UUID_RX, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  m_tx = service->createCharacteristic(UUID_TX, BLECharacteristic::PROPERTY_READ);
  service->start();
  m_server->startAdvertising();
  BLESERVERTRANSPORT_DBG(F("Server ready at ") << BLEDevice::getAddress().toString().data());
}

size_t
BleServerTransport::receive(uint8_t* buf, size_t bufSize, uint64_t* endpointId)
{
  std::string value = m_rx->getValue();
  if (value.empty()) {
    return 0;
  }
  m_rx->setValue("");

  if (value.size() > bufSize) {
    return 0;
  }

  memcpy(buf, value.data(), value.size());
  return value.size();
}

ndn_Error
BleServerTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  m_tx->setValue(const_cast<uint8_t*>(pkt), len);
  m_tx->notify();
}

} // namespace ndn
#endif // ESP32