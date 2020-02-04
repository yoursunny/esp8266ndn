#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_NRF52)

#include "../core/logger.hpp"
#include "ble-server-transport.hpp"
#include "ble-uuid.hpp"

#define LOG(...) LOGGER(BleServerTransport, __VA_ARGS__)

namespace esp8266ndn {

void
BleServerTransport::receiveImpl(const uint8_t* pkt, size_t pktLen, uint64_t endpointId)
{
  auto r = receiving();
  if (!r) {
    LOG(F("drop: no RX buffer"));
    return;
  }

  if (pktLen > r.bufLen()) {
    LOG(F("drop: RX buffer too short, pktLen=") << _DEC(pktLen));
    return;
  }

  std::copy_n(pkt, pktLen, r.buf());
  r(pktLen, endpointId);
}

#if defined(ARDUINO_ARCH_ESP32)

#define BLEUUID_FROM_ARRAY(array) BLEUUID(const_cast<uint8_t*>(array), sizeof(array), false)

BleServerTransport::BleServerTransport()
  : DynamicRxQueueMixin(512)
  , m_csCallbackHandler(*this)
{}

bool
BleServerTransport::begin(const char* deviceName)
{
  ::BLEDevice::init(deviceName);
  ::BLEDevice::setMTU(517);

  m_server = ::BLEDevice::createServer();
  m_svc = m_server->createService(BLEUUID_FROM_ARRAY(BLE_UUID_SVC));
  m_cs = m_svc->createCharacteristic(BLEUUID_FROM_ARRAY(BLE_UUID_CS),
                                     ::BLECharacteristic::PROPERTY_WRITE |
                                       ::BLECharacteristic::PROPERTY_NOTIFY);
  m_cs->setCallbacks(&m_csCallbackHandler);
  m_sc = m_svc->createCharacteristic(BLEUUID_FROM_ARRAY(BLE_UUID_SC),
                                     ::BLECharacteristic::PROPERTY_READ |
                                       ::BLECharacteristic::PROPERTY_NOTIFY);
  m_svc->start();

  auto adv = ::BLEDevice::getAdvertising();
  adv->addServiceUUID(BLEUUID_FROM_ARRAY(BLE_UUID_SVC));
  adv->setScanResponse(true);
  adv->start();
  return true;
}

String
BleServerTransport::getAddr() const
{
  return String(BLEDevice::getAddress().toString().data()) + " (addr-type=public)";
}

bool
BleServerTransport::doIsUp() const
{
  return m_server != nullptr && m_server->getConnectedCount() > 0;
}

void
BleServerTransport::doLoop()
{
  loopRxQueue();
}

bool
BleServerTransport::doSend(const uint8_t* pkt, size_t pktLen, uint64_t)
{
  if (m_sc == nullptr) {
    return false;
  }
  m_sc->setValue(const_cast<uint8_t*>(pkt), pktLen);
  m_sc->notify();
  return true;
}

BleServerTransport::CsCallbacks::CsCallbacks(BleServerTransport& transport)
  : m_transport(transport)
{}

void
BleServerTransport::CsCallbacks::onWrite(::BLECharacteristic* chr)
{
  if (chr != m_transport.m_cs) {
    return;
  }

  std::string value = chr->getValue();
  m_transport.receiveImpl(reinterpret_cast<const uint8_t*>(value.data()), value.size(), 0);
}

#elif defined(ARDUINO_ARCH_NRF52)

BleServerTransport::BleServerTransport()
  : DynamicRxQueueMixin(BLE_GATT_ATT_MTU_MAX)
  , BLEService(BLE_UUID_SVC)
  , m_cs(BLE_UUID_CS)
  , m_sc(BLE_UUID_SC)
{}

bool
BleServerTransport::begin(const char* deviceName)
{
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

err_t
BleServerTransport::begin()
{
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

String
BleServerTransport::getAddr() const
{
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

bool
BleServerTransport::doIsUp() const
{
  return Bluefruit.connected() > 0;
}

void
BleServerTransport::doLoop()
{
  loopRxQueue();
}

bool
BleServerTransport::doSend(const uint8_t* pkt, size_t pktLen, uint64_t)
{
  m_sc.write(pkt, pktLen);
  return m_sc.notify(pkt, pktLen);
}

void
BleServerTransport::handleCsWrite(uint16_t connHdl, ::BLECharacteristic* chr, uint8_t* pkt,
                                  uint16_t pktLen)
{
  BleServerTransport& self = static_cast<BleServerTransport&>(chr->parentService());
  self.receiveImpl(pkt, pktLen, connHdl);
}

#endif

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_NRF52)
