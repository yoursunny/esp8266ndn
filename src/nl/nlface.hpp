#ifndef ESP8266NDN_NLFACE_HPP
#define ESP8266NDN_NLFACE_HPP

#include "../transport/transport.hpp"

#include "../ndn-cpp/lite/name-lite.hpp"

#include <memory>

namespace ndn {

/** \brief A network face in NDN-Lite stack.
 */
class NlFace
{
public:
  explicit
  NlFace(Transport& transport);

  ~NlFace();

  /** \brief Register this face with NDN-Lite forwarder.
   */
  bool
  begin();

  /** \brief Unregister this face with NDN-Lite forwarder.
   */
  bool
  end();

  /** \brief Add a route in NDN-Lite FIB.
   *  \post Interests matching \p name are sent to the underlying transport.
   */
  ndn_Error
  addRoute(const NameLite& name);

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace ndn

#endif // ESP8266NDN_NLFACE_HPP
