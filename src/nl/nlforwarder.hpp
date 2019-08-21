#ifndef ESP8266NDN_NLFORWARDER_HPP
#define ESP8266NDN_NLFORWARDER_HPP

namespace ndn {

/** \brief NDN-Lite forwarder.
 *  \warning This is experimental. API may change without notice.
 */
class NlForwarderClass
{
public:
  /** \brief Initialize NDN-Lite forwarder.
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

extern NlForwarderClass NlForwarder;

} // namespace ndn

#endif // ESP8266NDN_NLFORWARDER_HPP
