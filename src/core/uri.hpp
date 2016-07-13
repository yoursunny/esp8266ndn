#ifndef ESP8266NDN_URI_HPP
#define ESP8266NDN_URI_HPP

#include "../ndn-cpp/lite/name-lite.hpp"
#include <Printable.h>

namespace ndn {

/** \brief parse name from ndn: URI
 *  \param[out] name the name
 *  \param[inout] uri null-terminated URI; will be overwritten as buffer for name components
 *  \return whether success
 */
bool
parseNameFromUri(NameLite& name, char* uri);

/** \brief parse name component from URI segment
 *  \param[inout] uri null-terminated URI segment; will be overwritten as buffer for name component
 */
NameLite::Component
parseNameComponentFromUri(char* uri);

/** \brief allows a Name to be printed as URI
 */
class PrintUri : public Printable
{
public:
  explicit
  PrintUri(const NameLite& name);

  virtual size_t
  printTo(Print& p) const override;

private:
  size_t
  printTo(Print& p, const NameLite::Component& comp) const;

private:
  const NameLite& m_name;
};

} // namespace ndn

#endif // ESP8266NDN_URI_HPP
