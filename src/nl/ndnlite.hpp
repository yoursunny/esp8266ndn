#ifndef ESP8266NDN_NDNLITE_HPP
#define ESP8266NDN_NDNLITE_HPP

namespace ndn {

/** \brief Integration with NDN-Lite stack.
 *  \warning This integration is experiment. API may change without notice.
 */
class NdnLiteClass
{
public:
  /** \brief Initialize NDN-Lite stack, including security and forwarder.
   */
  void
  begin();

  /** \brief Execute NDN-Lite event loop once.
   *
   *  This must be invoked in Arduino's main loop().
   */
  void
  loop();
};

extern NdnLiteClass NdnLite;

} // namespace ndn

#endif // ESP8266NDN_NDNLITE_HPP
