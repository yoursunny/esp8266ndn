#ifndef ESP8266NDN_NLINTF_HPP
#define ESP8266NDN_NLINTF_HPP

#include "../../ndn-cpp/lite/name-lite.hpp"

#include "../../ndn-lite/forwarder/face.h"

namespace ndn {
namespace detail {

class NlIntf : public ndn_face_intf_t
{
public:
  NlIntf(int intfType, ndn_face_intf_send sendFunc);

  ~NlIntf();

  bool
  begin();

  bool
  end();

  ndn_Error
  addRoute(const NameLite& name);

protected:
  int
  intfReceive(const uint8_t* pkt, size_t len);
};

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_NLINTF_HPP
