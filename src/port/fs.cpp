#include "fs.hpp"
#include <algorithm>

#if defined(ARDUINO_ARCH_ESP8266)
#include <LittleFS.h>
#define FSPORT_FILESYSTEM (::LittleFS)
#define FSPORT_READ ("r")
#define FSPORT_WRITE ("w")
#elif defined(ARDUINO_ARCH_ESP32)
#include <FFat.h>
#define FSPORT_FILESYSTEM (::FFat)
#define FSPORT_READ (FILE_READ)
#define FSPORT_WRITE (FILE_WRITE)
#elif defined(ARDUINO_ARCH_NRF52)
#include <InternalFileSystem.h>
#define FSPORT_FILESYSTEM (::InternalFS)
#define FSPORT_READ (::Adafruit_LittleFS_Namespace::FILE_O_READ)
#define FSPORT_WRITE (::Adafruit_LittleFS_Namespace::FILE_O_WRITE)
#endif

namespace esp8266ndn {
namespace ndnph_port {

bool
FileStore::open(const char* path)
{
  size_t pathLen = strlen(path);
  if (pathLen == 0 || pathLen > maxNameLen || path[0] != '/' || path[pathLen - 1] == '/') {
    return false;
  }
  FSPORT_FILESYSTEM.mkdir(path);
  strncpy(m_path, path, maxNameLen + 1);
  m_path[pathLen++] = '/';
  m_pathLen = pathLen;
  return true;
}

int
FileStore::read(const char* filename, uint8_t* buffer, size_t count)
{
  if (!joinPath(filename)) {
    return -1;
  }

  auto file = FSPORT_FILESYSTEM.open(m_path, FSPORT_READ);
  if (!file) {
    return -1;
  }

  auto size = file.size();
  file.read(buffer, std::min<decltype(size)>(size, count));
  file.close();
  return size;
}

bool
FileStore::write(const char* filename, const uint8_t* buffer, size_t count)
{
  if (!joinPath(filename)) {
    return false;
  }

  auto file = FSPORT_FILESYSTEM.open(m_path, FSPORT_WRITE);
  if (!file) {
    return false;
  }

  size_t nWrite = file.write(buffer, count);
  file.close();
  return nWrite == count;
}

bool
FileStore::unlink(const char* filename)
{
  if (!joinPath(filename)) {
    return false;
  }

  FSPORT_FILESYSTEM.remove(m_path);
  return !FSPORT_FILESYSTEM.exists(m_path);
}

bool
FileStore::joinPath(const char* filename)
{
  size_t nameLen = strlen(filename);
  if (m_pathLen == 0 || nameLen == 0 || nameLen > maxNameLen) {
    return false;
  }
  strncpy(&m_path[m_pathLen], filename, sizeof(m_path) - m_pathLen);
  return true;
}

} // namespace ndnph_port
} // namespace esp8266ndn
