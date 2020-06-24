#ifndef ESP8266NDN_PORT_FS_HPP
#define ESP8266NDN_PORT_FS_HPP

#include <cstdint>
#include <cstdlib>

namespace esp8266ndn {
namespace ndnph_port {

/** @brief File storage on microcontroller filesystem. */
class FileStore
{
public:
  bool open(const char* path);

  int read(const char* filename, uint8_t* buffer, size_t count);

  bool write(const char* filename, const uint8_t* buffer, size_t count);

  bool unlink(const char* filename);

private:
  bool joinPath(const char* filename);

private:
  char m_path[64];
  size_t m_pathLen = 0;
};

} // namespace ndnph_port
} // namespace esp8266ndn

namespace ndnph {
namespace port {
using FileStore = esp8266ndn::ndnph_port::FileStore;
} // namespace port
} // namespace ndnph

#endif // ESP8266NDN_PORT_FS_HPP
