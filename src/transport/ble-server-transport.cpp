#include "ble-server-transport.hpp"
#include "../core/logger.hpp"

#if defined(ESP32)
#include <BLEDevice.h>
#elif defined(ARDUINO_ARCH_NRF52)
#include <cassert>
#include <bluefruit.h>
#endif

#define BLESERVERTRANSPORT_DBG(...) DBG(BleServerTransport, __VA_ARGS__)

namespace ndn {

static uint8_t UUID_SVC[] = {
  0x91, 0x73, 0xd9, 0x84, 0x50, 0x39, 0x24, 0x88, 0x2a, 0x41, 0x88, 0x07, 0xe3, 0x77, 0x95, 0x09
}; // 099577e3-0788-412a-8824-395084d97391
static uint8_t UUID_RX[] = {
  0x49, 0x1f, 0xa8, 0xa6, 0x95, 0x2f, 0x51, 0xa3, 0xd8, 0x46, 0x41, 0xa5, 0x89, 0xbb, 0x5a, 0xcc
}; // cc5abb89-a541-46d8-a351-2f95a6a81f49
static uint8_t UUID_TX[] = {
  0xe4, 0xbd, 0x73, 0xfc, 0xb2, 0xb1, 0x5d, 0xb9, 0x61, 0x42, 0x83, 0x0d, 0x27, 0x95, 0x2f, 0x97
}; // 972f9527-0d83-4261-b95d-b1b2fc73bde4

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
  m_service = m_server->createService(BLEUUID(UUID_SVC, sizeof(UUID_SVC), false));
  m_rx = m_service->createCharacteristic(BLEUUID(UUID_RX, sizeof(UUID_RX), false),
         BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  m_tx = m_service->createCharacteristic(BLEUUID(UUID_TX, sizeof(UUID_TX), false),
         BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  m_service->start();
  m_server->startAdvertising();
  BLESERVERTRANSPORT_DBG("Server ready at " << BLEDevice::getAddress().toString().data() <<
                         " (public address)");
  return true;
#elif defined(ARDUINO_ARCH_NRF52)
  Bluefruit.configPrphConn(BLE_GATT_ATT_MTU_MAX, BLE_GAP_EVENT_LENGTH_DEFAULT, BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT, BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT);
  Bluefruit.begin();
  Bluefruit.setName(deviceName);
  Bluefruit.setTxPower(4);

  m_service = new BLEService(UUID_SVC);
  err_t e = m_service->begin();
  if (e != 0) {
    BLESERVERTRANSPORT_DBG("service.begin error " << e);
    return false;
  }

  m_rx = new BLECharacteristic(UUID_RX);
  m_rx->setMaxLen(BLE_GATT_ATT_MTU_MAX - 3);
  m_rx->setProperties(CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
  m_rx->setPermission(SECMODE_OPEN, SECMODE_OPEN);
  e = m_rx->begin();
  if (e != 0) {
    BLESERVERTRANSPORT_DBG("characteristic(RX).begin error " << e);
    return false;
  }

  m_tx = new BLECharacteristic(UUID_TX);
  m_tx->setMaxLen(BLE_GATT_ATT_MTU_MAX - 3);
  m_tx->setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  m_tx->setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  e = m_tx->begin();
  if (e != 0) {
    BLESERVERTRANSPORT_DBG("characteristic(TX).begin error " << e);
    return false;
  }

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(*m_service);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start();

  uint8_t mac[6]; // reversed
  uint8_t addrType = Bluefruit.Gap.getAddr(mac);
  if (addrType != BLE_GAP_ADDR_TYPE_RANDOM_STATIC) {
    BLESERVERTRANSPORT_DBG("Gap.getAddr unexpected addr type " << addrType);
    return false;
  }
  char addr[18];
  snprintf(addr, sizeof(addr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  BLESERVERTRANSPORT_DBG("Server ready at " << addr << " (random-static address)");
  return true;
#else
  BLESERVERTRANSPORT_DBG("no BLE library on this platform");
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
  m_rx->setValue(buf, 0);

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
