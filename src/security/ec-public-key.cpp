#include "ec-public-key.hpp"
#include "detail/ec-impl.hpp"
#include "../ndn-cpp/lite/util/crypto-lite.hpp"

#include <algorithm>
#include <cstring>

#ifdef ARDUINO_ARCH_NRF52
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

namespace ndn {

EcPublicKey::EcPublicKey() = default;

EcPublicKey::~EcPublicKey() = default;

bool
EcPublicKey::import(const uint8_t bits[65])
{
  m_impl.reset(new Impl(bits));
  return true;
}

static const uint8_t OID_ECPUBKEY[] PROGMEM {
  0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01
};
bool
EcPublicKey::importCert(const DataLite& data)
{
  if (data.getMetaInfo().getType() != ndn_ContentType_KEY) {
    return false;
  }
  const BlobLite& content = data.getContent();
  if (content.isNull() || content.size() < 65) {
    return false;
  }
#ifdef ESP8266
  if (memmem_P(content.buf(), content.size(), OID_ECPUBKEY, sizeof(OID_ECPUBKEY)) == nullptr) {
    return false;
  }
#endif // ESP8266
#ifdef ESP32
  if (std::search(content.buf(), content.buf() + content.size(),
                  OID_ECPUBKEY, OID_ECPUBKEY + sizeof(OID_ECPUBKEY)) ==
      content.buf() + content.size()) {
    return false;
  }
#endif // ESP32
  const uint8_t* bits = content.buf() + content.size() - 65;
  if (bits[0] != 0x04) {
    return false;
  }
  return this->import(bits);
}

bool
EcPublicKey::verify(const uint8_t* input, size_t inputLen, const uint8_t* sig, size_t sigLen) const
{
  if (!m_impl) {
    return false;
  }
  uint8_t hash[ndn_SHA256_DIGEST_SIZE];
  CryptoLite::digestSha256(input, inputLen, hash);
  return m_impl->verify(hash, sig, sigLen);
}

} // namespace ndn
