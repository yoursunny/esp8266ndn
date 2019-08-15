#ifndef ESP8266NDN_NDNLITE_HPP
#define ESP8266NDN_NDNLITE_HPP

namespace ndn {

class NdnLiteClass
{
public:
  void
  begin();

  void
  loop();
};

extern NdnLiteClass NdnLite;

} // namespace ndn

#endif // ESP8266NDN_NDNLITE_HPP
