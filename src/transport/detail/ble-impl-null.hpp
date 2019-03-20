#ifndef ESP8266NDN_BLE_IMPL_NULL_HPP
#define ESP8266NDN_BLE_IMPL_NULL_HPP

#include <WString.h>

namespace ndn {

class BleServiceImpl;

class BleDeviceImplClass
{
public:
  int
  init(const char* deviceName, bool enableServer, int nClients)
  {
    return __LINE__;
  }

  String
  getAddr()
  {
    return F("00:00:00:00:00:00");
  }

  int
  advertiseService(BleServiceImpl* service)
  {
    return __LINE__;
  }
};

extern BleDeviceImplClass BleDeviceImpl;

class BleServiceImpl
{
public:
  BleServiceImpl()
  {
  }

  int
  begin()
  {
    return __LINE__;
  }

  size_t
  receive(uint8_t* buf, size_t bufSize)
  {
    return 0;
  }

  void
  send(const uint8_t* pkt, size_t len)
  {
  }
};

} // namespace ndn

#endif // ESP8266NDN_BLE_IMPL_NULL_HPP
