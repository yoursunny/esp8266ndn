#ifndef ESP8266NDN_NLTRANSPORT_HPP
#define ESP8266NDN_NLTRANSPORT_HPP

#include "../transport/transport.hpp"

#include "../ndn-cpp/lite/name-lite.hpp"

#include <memory>

namespace ndn {

class NlTransport : public Transport
{
public:
  NlTransport();

  ~NlTransport();

  bool
  begin();

  bool
  end();

  ndn_Error
  addRoute(const NameLite& name);

  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) override;

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // ESP8266NDN_NLTRANSPORT_HPP
