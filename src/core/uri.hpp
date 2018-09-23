#ifndef ESP8266NDN_URI_HPP
#define ESP8266NDN_URI_HPP

#include "../ndn-cpp/lite/name-lite.hpp"
#include <Printable.h>

namespace ndn {

/** \brief Parse name from ndn: URI.
 *  \param[out] name the name
 *  \param[inout] uri null-terminated URI; will be overwritten as buffer for name components
 *  \return whether success
 *  \warning This function may accept certain invalid URIs.
 */
bool
parseNameFromUri(NameLite& name, char* uri);

/** \brief Parse name component from URI segment.
 *  \param[inout] uri null-terminated URI segment; will be overwritten as buffer for name component
 *  \warning This function may accept certain invalid URIs.
 */
NameLite::Component
parseNameComponentFromUri(char* uri);

/** \brief Print a Name as URI.
 */
class PrintUri : public Printable
{
public:
  explicit
  PrintUri(const NameLite& name);

  size_t
  printTo(Print& p) const override;

private:
  size_t
  printTo(Print& p, const NameLite::Component& comp) const;

private:
  const NameLite& m_name;
};

} // namespace ndn

#endif // ESP8266NDN_URI_HPP
