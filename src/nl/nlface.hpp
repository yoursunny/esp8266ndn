#ifndef ESP8266NDN_NLFACE_HPP
#define ESP8266NDN_NLFACE_HPP

#include "../transport/transport.hpp"

#include "../ndn-cpp/lite/name-lite.hpp"

#include <memory>

namespace ndn {

class NlFace
{
public:
  NlFace(Transport& transport);

  ~NlFace();

  bool
  begin();

  bool
  end();

  ndn_Error
  addRoute(const NameLite& name);

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // ESP8266NDN_NLFACE_HPP
