#include "ble-server-transport.hpp"
#include "../core/logger.hpp"

#if defined(ESP32)
#include <BLEDevice.h>
#elif defined(ARDUINO_ARCH_NRF52)
#include <cassert>
#include <bluefruit.h>
#endif

#define BLESERVERTRANSPORT_DBG(...) DBG(BleServerTransport, __VA_ARGS__)

#define UUID_SVC "099577e3-0788-412a-8824-395084d97391"
#define UUID_RX  "cc5abb89-a541-46d8-a351-2f95a6a81f49"
#define UUID_TX  "972f9527-0d83-4261-b95d-b1b2fc73bde4"

// 128-bit UUID appears to be broken on nRF52.
// The actual UUIDs on nRF52 are FF00 FF01 FF02.
// TODO further investigate

namespace ndn {

#if defined(ARDUINO_ARCH_NRF52)
static BLEUuid
parseBleUuid128(const char* value)
{
  assert(strlen(value) == 36);
  uint8_t uuid128[16];
  int n = 0;
  for(int i = 0; i < 36;) {
    if (value[i] == '-') {
      ++i;
    }
    uint8_t msb = value[i];
    uint8_t lsb = value[i + 1];
    if (msb > '9') msb -= 7;
    if (lsb > '9') lsb -= 7;
    uuid128[15 - n++] = ((msb & 0x0F) << 4) | (lsb & 0x0F);
    i += 2;
  }
  return BLEUuid(uuid128);
}
#endif

BleServerTransport::BleServerTransport()
  : m_server(nullptr)
  , m_service(nullptr)
  , m_rx(nullptr)
  , m_tx(nullptr)
{
}

bool
BleServerTransport::begin(const char* deviceName)
{
#if defined(ESP32)
  BLEDevice::init(deviceName);
  BLEDevice::setMTU(517);
  m_server = BLEDevice::createServer();
  m_service = m_server->createService(UUID_SVC);
  m_rx = m_service->createCharacteristic(UUID_RX, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  m_tx = m_service->createCharacteristic(UUID_TX, BLECharacteristic::PROPERTY_READ);
  m_service->start();
  m_server->startAdvertising();
  BLESERVERTRANSPORT_DBG(F("Server ready at ") << BLEDevice::getAddress().toString().data());
  return true;
#elif defined(ARDUINO_ARCH_NRF52)
  Bluefruit.configPrphConn(BLE_GATT_ATT_MTU_MAX, BLE_GAP_EVENT_LENGTH_DEFAULT, BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT, BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT);
  Bluefruit.begin();
  Bluefruit.setName(deviceName);
  Bluefruit.setTxPower(4);

  // m_service = new BLEService(parseBleUuid128(UUID_SVC));
  m_service = new BLEService(0xFF00);
  err_t e = m_service->begin();
  if (e != 0) {
    BLESERVERTRANSPORT_DBG(F("service.begin error ") << e);
    return false;
  }
  // m_rx = new BLECharacteristic(parseBleUuid128(UUID_RX));
  m_rx = new BLECharacteristic(0xFF01);
  m_rx->setMaxLen(BLE_GATT_ATT_MTU_MAX - 3);
  m_rx->setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
  m_rx->setPermission(SECMODE_OPEN, SECMODE_OPEN);
  e = m_rx->begin();
  if (e != 0) {
    BLESERVERTRANSPORT_DBG(F("characteristic.begin error ") << e);
    return false;
  }
  // m_tx = new BLECharacteristic(parseBleUuid128(UUID_TX));
  m_tx = new BLECharacteristic(0xFF02);
  m_tx->setMaxLen(BLE_GATT_ATT_MTU_MAX - 3);
  m_tx->setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  m_tx->setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  e = m_tx->begin();
  if (e != 0) {
    BLESERVERTRANSPORT_DBG(F("characteristic.begin error ") << e);
    return false;
  }

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(*m_service);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start();

  uint8_t mac[6]; // reversed
  Bluefruit.Gap.getAddr(mac);
  char addr[18];
  snprintf(addr, sizeof(addr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  BLESERVERTRANSPORT_DBG(F("Server ready at ") << addr);
  return true;
#else
  BLESERVERTRANSPORT_DBG(F("no BLE library on this platform"));
  return true;
#endif
}

size_t
BleServerTransport::receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId)
{
  endpointId = 0;

#if defined(ESP32)
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
#elif defined(ARDUINO_ARCH_NRF52)
  uint16_t len = m_rx->read(buf, bufSize);
  m_rx->write("", 0);
  return len;
#else
  return 0;
#endif
}

ndn_Error
BleServerTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
#if defined(ESP32)
  m_tx->setValue(const_cast<uint8_t*>(pkt), len);
  m_tx->notify();
  return NDN_ERROR_success;
#elif defined(ARDUINO_ARCH_NRF52)
  m_tx->write(pkt, len);
  m_tx->notify(pkt, len);
  return NDN_ERROR_success;
#else
  return NDN_ERROR_SocketTransport_socket_is_not_open;
#endif
}

} // namespace ndn
