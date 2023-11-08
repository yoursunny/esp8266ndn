#ifndef ESP8266NDN_PORT_SHA256_CRYPTOSUITE_HPP
#define ESP8266NDN_PORT_SHA256_CRYPTOSUITE_HPP

#include "../vendor/cryptosuite-sha256.h"
#include <algorithm>

namespace esp8266ndn {
namespace ndnph_port {

/** @brief SHA256 algorithm, implemented with Cryptosuite. */
class Sha256 {
public:
  Sha256() {
    m_sha.init();
  }

  void update(const uint8_t* chunk, size_t size) {
    m_sha.write(chunk, size);
  }

  bool final(uint8_t* digest) {
    std::copy_n(m_sha.result(), 32, digest);
    return true;
  }

private:
  ::Sha256 m_sha;
};

/** @brief HMAC-SHA256 algorithm, implemented with Cryptosuite. */
class HmacSha256 {
public:
  explicit HmacSha256(const uint8_t* key, size_t keyLen) {
    m_sha.initHmac(key, keyLen);
  }

  void update(const uint8_t* chunk, size_t size) {
    m_sha.write(chunk, size);
  }

  bool final(uint8_t* result) {
    std::copy_n(m_sha.resultHmac(), HASH_LENGTH, result);
    m_sha.reset();
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
using HmacSha256 = esp8266ndn::ndnph_port::HmacSha256;
} // namespace port
} // namespace ndnph

#undef HASH_LENGTH
#undef BLOCK_LENGTH

#endif // ESP8266NDN_PORT_SHA256_CRYPTOSUITE_HPP
