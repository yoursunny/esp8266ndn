#ifndef ESP8266NDN_PORT_SHA256_BEARSSL_HPP
#define ESP8266NDN_PORT_SHA256_BEARSSL_HPP

#include <bearssl/bearssl_hash.h>

namespace esp8266ndn {
namespace ndnph_port {

/** @brief SHA256 algorithm, implemented with BearSSL. */
class Sha256
{
public:
  Sha256()
  {
    ::br_sha256_init(&m_ctx);
  }

  void update(const uint8_t* chunk, size_t size)
  {
    ::br_sha256_update(&m_ctx, chunk, size);
  }

  bool final(uint8_t* digest)
  {
    ::br_sha256_out(&m_ctx, digest);
    return true;
  }

private:
  ::br_sha256_context m_ctx;
};

} // namespace ndnph_port
} // namespace esp8266ndn

namespace ndnph {
namespace port {
using Sha256 = esp8266ndn::ndnph_port::Sha256;
} // namespace port
} // namespace ndnph

#endif // ESP8266NDN_PORT_SHA256_BEARSSL_HPP
