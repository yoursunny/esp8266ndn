#include "uri.hpp"
#include <Print.h>

namespace ndn {

bool
parseNameFromUri(NameLite& name, char* uri)
{
  name.clear();
  char* token = strtok(uri, "/");
  while (token != nullptr) {
    if (!(token == uri && strcmp(token, "ndn:") == 0)) {
      ndn_Error res = name.append(parseNameComponentFromUri(token));
      if (res != NDN_ERROR_success) {
        return false;
      }
    }
    token = strtok(nullptr, "/");
  }
  return true;
}

// strspn is missing: https://github.com/esp8266/Arduino/issues/572
// http://clc-wiki.net/mediawiki/index.php?title=C_standard_library:string.h:strspn&oldid=4237
static size_t
my_strspn(const char* s1, const char* s2)
{
  size_t ret = 0;
  while (*s1 && strchr(s2, *s1++)) {
    ret++;
  }
  return ret;
}

NameLite::Component
parseNameComponentFromUri(char* uri)
{
  size_t len = strlen(uri);
  if (len >= 3 && my_strspn(uri, ".") == len) {
    return NameLite::Component(reinterpret_cast<uint8_t*>(uri), len - 3);
  }

  size_t opos = 0;
  for (size_t i = 0; i < len; ++i) {
    char ch = uri[i];
    if (ch == '%' && i + 2 < len) {
      uri[i] = uri[i + 1];
      uri[i + 1] = uri[i + 2];
      uri[i + 2] = '\0';
      uri[opos++] = static_cast<char>(strtol(&uri[i], nullptr, 16));
      i += 2;
    }
    else {
      uri[opos++] = ch;
    }
  }
  return NameLite::Component(reinterpret_cast<uint8_t*>(uri), opos);
}

PrintUri::PrintUri(const NameLite& name)
  : m_name(name)
{
}

size_t
PrintUri::printTo(Print& p) const
{
  if (m_name.size() == 0) {
    return p.print('/');
  }

  size_t len = 0;
  for (size_t i = 0; i < m_name.size(); ++i) {
    len += p.print('/');
    len += this->printTo(p, m_name.get(i));
  }
  return len;
}

size_t
PrintUri::printTo(Print& p, const NameLite::Component& comp) const
{
  const BlobLite& blob = comp.getValue();
  const uint8_t* buf = blob.buf();
  size_t sz = blob.size();

  size_t len = 0;
  size_t nPeriods = 0;
  for (size_t i = 0; i < sz; ++i) {
    char ch = static_cast<char>(buf[i]);
    switch (ch) {
      case '.':
        ++nPeriods;
        // fallthrough
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '+':
      case '_':
      case '-':
        len += p.print(ch);
        break;
      default:
        len += p.printf("%%%02X", ch);
        break;
    }
  }

  if (nPeriods == sz) {
    len += p.print("...");
  }

  return len;
}

} // namespace ndn