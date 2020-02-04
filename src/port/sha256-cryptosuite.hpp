#ifndef ESP8266NDN_PORT_SHA256_CRYPTOSUITE_HPP
#define ESP8266NDN_PORT_SHA256_CRYPTOSUITE_HPP

#include "../vendor/cryptosuite-sha256.h"
#include <algorithm>

namespace esp8266ndn {
namespace ndnph_port {

/** @brief SHA256 algorithm, implemented with Cryptosuite. */
class Sha256
{
public:
  Sha256()
  {
    m_sha.init();
  }

  void update(const uint8_t* chunk, size_t size)
  {
    m_sha.write(chunk, size);
  }

  bool final(uint8_t* digest)
  {
    std::copy_n(m_sha.result(), 32, digest);
    return true;
  }

private:
  ::Sha256 m_sha;
};

} // namespace ndnph_port
} // namespace esp8266ndn

namespace ndnph {
namespace port {
using Sha256 = esp8266ndn::ndnph_port::Sha256;
} // namespace port
} // namespace ndnph

#endif // ESP8266NDN_PORT_SHA256_CRYPTOSUITE_HPP
