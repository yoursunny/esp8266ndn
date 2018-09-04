#include "crypto-memory.hpp"
#include "../../ndn-cpp/c/util/crypto.h"
#include <Arduino.h>

ndn_Error
ndn_generateRandomBytes(uint8_t* buffer, size_t bufferLength)
{
  for (size_t i = 0; i < bufferLength; ++i) {
    buffer[i] = random(256); // use hardware RNG
  }
  return NDN_ERROR_success;
}

int
ndn_verifyHmacWithSha256Signature(const uint8_t* key, size_t keyLength,
                                  const uint8_t* signature, size_t signatureLength,
                                  const uint8_t* data, size_t dataLength)
{
  if (signatureLength != ndn_SHA256_DIGEST_SIZE) {
    return 0;
  }
  uint8_t newSig[ndn_SHA256_DIGEST_SIZE];
  ndn_computeHmacWithSha256(key, keyLength, data, dataLength, newSig);
  return ndn::compareDigest(signature, newSig, ndn_SHA256_DIGEST_SIZE);
}

#if defined(ESP8266)

#include <bearssl/bearssl_hash.h>
#include <bearssl/bearssl_hmac.h>

void
ndn_digestSha256(const uint8_t* data, size_t dataLength, uint8_t* digest)
{
  br_sha256_context ctx;
  br_sha256_init(&ctx);
  br_sha256_update(&ctx, data, dataLength);
  br_sha256_out(&ctx, digest);
}

void
ndn_computeHmacWithSha256(const uint8_t* key, size_t keyLength, const uint8_t* data,
                          size_t dataLength, uint8_t* digest)
{
  br_hmac_key_context keyCtx;
  br_hmac_key_init(&keyCtx, &br_sha256_vtable, key, keyLength);
  br_hmac_context ctx;
  br_hmac_init(&ctx, &keyCtx, 0);
  br_hmac_update(&ctx, data, dataLength);
  br_hmac_out(&ctx, digest);
}

#elif defined(ESP32)

#include <mbedtls/md.h>
#include <mbedtls/sha256.h>

void
ndn_digestSha256(const uint8_t* data, size_t dataLength, uint8_t* digest)
{
  mbedtls_sha256_context ctx;
  mbedtls_sha256_init(&ctx);
  mbedtls_sha256_starts_ret(&ctx, false);
  mbedtls_sha256_update_ret(&ctx, data, dataLength);
  mbedtls_sha256_finish_ret(&ctx, digest);
  mbedtls_sha256_free(&ctx);
}

void
ndn_computeHmacWithSha256(const uint8_t* key, size_t keyLength, const uint8_t* data,
                          size_t dataLength, uint8_t* digest)
{
  static const mbedtls_md_info_t* sha256 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  mbedtls_md_hmac(sha256, key, keyLength, data, dataLength, digest);
  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, sha256, 1);
  mbedtls_md_hmac_starts(&ctx, key, keyLength);
  mbedtls_md_hmac_update(&ctx, data, dataLength);
  mbedtls_md_hmac_finish(&ctx, digest);
  mbedtls_md_free(&ctx);
}

#endif