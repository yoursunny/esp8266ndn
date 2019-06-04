/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2015-2019 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU Lesser General Public License is in the file COPYING.
 */

#include "../../c/util/crypto.h"
#include "../../lite/util/crypto-lite.hpp"

#if 0

#include <Arduino.h>

// Define this in a .cpp file since the random() function is overloaded.
ndn_Error
ndn_generateRandomBytes(uint8_t *buffer, size_t bufferLength)
{
  // Assume the application has already initialized it, e.g.:
  // randomSeed(analogRead(0));
  size_t i;
  for (i = 0; i < bufferLength; ++i)
    buffer[i] = random(0, 256);

  return NDN_ERROR_success;
}

#endif

namespace ndn {

void
CryptoLite::digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest)
{
  ndn_digestSha256(data, dataLength, digest);
}

ndn_Error
CryptoLite::generateRandomBytes(uint8_t *buffer, size_t bufferLength)
{
  return ndn_generateRandomBytes(buffer, bufferLength);
}

ndn_Error
CryptoLite::generateRandomFloat(float& value)
{
  ndn_Error error;
  uint32_t random;
  if ((error = generateRandomBytes((uint8_t*)&random, sizeof(random))))
    return error;

  value = ((float)random) / 0xffffffff;
  return NDN_ERROR_success;
}

#if NDN_CPP_HAVE_LIBCRYPTO

void
CryptoLite::computeHmacWithSha256
  (const uint8_t *key, size_t keyLength, const uint8_t *data, size_t dataLength,
   uint8_t *digest)
{
  ndn_computeHmacWithSha256(key, keyLength, data, dataLength, digest);
}

bool
CryptoLite::verifyHmacWithSha256Signature
  (const uint8_t *key, size_t keyLength, const uint8_t* signature,
   size_t signatureLength, const uint8_t *data, size_t dataLength)
{
  return ndn_verifyHmacWithSha256Signature
    (key, keyLength, signature, signatureLength, data, dataLength) != 0;
}

void
CryptoLite::computePbkdf2WithHmacSha1
  (const uint8_t* password, size_t passwordLength, const uint8_t* salt,
   size_t saltLength, int nIterations, size_t resultLength, uint8_t* result)
{
  ndn_computePbkdf2WithHmacSha1
    (password, passwordLength, salt, saltLength, nIterations, resultLength,
     result);
}

#endif

bool
CryptoLite::verifyDigestSha256Signature
  (const uint8_t* signature, size_t signatureLength, const uint8_t *data,
   size_t dataLength)
{
  return ndn_verifyDigestSha256Signature
    (signature, signatureLength, data, dataLength) != 0;
}

uint32_t
CryptoLite::murmurHash3
  (uint32_t nHashSeed, const uint8_t* dataToHash, size_t dataToHashLength)
{
  return 0;//(nHashSeed, dataToHash, dataToHashLength);
}

uint32_t
CryptoLite::murmurHash3(uint32_t nHashSeed, uint32_t value)
{
  return 0;//(nHashSeed, (const uint8_t*)&value, sizeof(uint32_t));
}

}

#if 0
// Put the ARDUINO implementations in this C++ file, not in crypto.c.

#include "../../../contrib/cryptosuite/sha256.h"

void
ndn_digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest)
{
  // The Arduino is single-threaded, so use the global Sha256 object.
  Sha256.init();
  for (size_t i = 0; i < dataLength; ++i)
    Sha256.write(data[i]);
  memcpy(digest, Sha256.result(), ndn_SHA256_DIGEST_SIZE);
}

void
ndn_computeHmacWithSha256
  (const uint8_t *key, size_t keyLength, const uint8_t *data, size_t dataLength,
   uint8_t *digest)
{
  // The Arduino is single-threaded, so use the global Sha256 object.
  Sha256.initHmac(key, keyLength);
  for (size_t i = 0; i < dataLength; ++i)
    Sha256.write(data[i]);
  memcpy(digest, Sha256.resultHmac(), ndn_SHA256_DIGEST_SIZE);
}

#endif // ARDUINO
