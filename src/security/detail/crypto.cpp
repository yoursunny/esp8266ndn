#include "../../ndn-cpp/c/util/crypto.h"
#include <Arduino.h>
#include "../../core/detail/fix-maxmin.hpp"

ndn_Error
ndn_generateRandomBytes(uint8_t* buffer, size_t bufferLength)
{
  for (size_t i = 0; i < bufferLength; ++i) {
    buffer[i] = random(256); // use hardware RNG
  }
  return NDN_ERROR_success;
}

#if defined(ESP8266)

#include <bearssl/bearssl_hash.h>

void
ndn_digestSha256(const uint8_t* data, size_t dataLength, uint8_t* digest)
{
  br_sha256_context ctx;
  br_sha256_init(&ctx);
  br_sha256_update(&ctx, data, dataLength);
  br_sha256_out(&ctx, digest);
}

#elif defined(ESP32)

#include <mbedtls/sha256.h>

void
ndn_digestSha256(const uint8_t* data, size_t dataLength, uint8_t* digest)
{
  mbedtls_sha256_ret(data, dataLength, digest, 0);
}

#endif
