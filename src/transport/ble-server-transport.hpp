#ifndef ESP8266NDN_BLE_SERVER_TRANSPORT_HPP
#define ESP8266NDN_BLE_SERVER_TRANSPORT_HPP

#include "transport.hpp"

class BLEServer;
class BLEService;
class BLECharacteristic;

namespace ndn {

/** \brief a transport that acts as a Bluetooth Low Energy server
 */
class BleServerTransport : public Transport
{
public:
  BleServerTransport();

  bool
  begin(const char* deviceName);

  size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId) final;

  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) final;

private:
  BLEServer* m_server;
  BLEService* m_service;
  BLECharacteristic* m_rx;
  BLECharacteristic* m_tx;
};

} // namespace ndn

#endif // ESP8266NDN_BLE_SERVER_TRANSPORT_HPP
