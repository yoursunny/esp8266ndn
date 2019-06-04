/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2019 Regents of the University of California.
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

#ifndef NDN_CRYPTOLITE_HPP
#define NDN_CRYPTOLITE_HPP

#include "blob-lite.hpp"
#include "../../c/errors.h"

namespace ndn {

/**
 * CryptoLite has static methods for basic cryptography operations.
 */
class CryptoLite {
public:
  /**
   * Compute the sha-256 digest of data.
   * @param data Pointer to the input byte array.
   * @param dataLength The length of data.
   * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to
   * receive the digest.
   */
  static void
  digestSha256(const uint8_t* data, size_t dataLength, uint8_t *digest);

  /**
   * Compute the sha-256 digest of data.
   * @param data The input byte array.
   * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to
   * receive the digest.
   */
  static void
  digestSha256(const BlobLite& data, uint8_t *digest)
  {
    digestSha256(data.buf(), data.size(), digest);
  }

  /**
   * Fill the buffer with random bytes.
   * @param buffer Write the random bytes to this buffer.
   * @param bufferLength The number of bytes to write to buffer.
   * @return 0 for success, else NDN_ERROR_Error_in_generate_operation for an
   * error including if the random number generator is not seeded.
   */
  static ndn_Error
  generateRandomBytes(uint8_t* buffer, size_t bufferLength);

  /**
   * Generate a random value from 0.0 to 1.0.
   * @param value Set value to the random value.
   * @return 0 for success, else NDN_ERROR_Error_in_generate_operation for an
   * error including if the random number generator is not seeded.
   */
  static ndn_Error
  generateRandomFloat(float& value);

  /**
   * Compute the HMAC with sha-256 of data, as defined in
   * http://tools.ietf.org/html/rfc2104#section-2 .
   * @param key A pointer to buffer with the key.
   * @param keyLength The length of key.
   * @param data A pointer to the input byte array.
   * @param dataLength The length of data.
   * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to receive
   * the HMAC digest.
   */
  static void
  computeHmacWithSha256
    (const uint8_t* key, size_t keyLength, const uint8_t* data, size_t dataLength,
     uint8_t *digest);

  /**
   * Compute the HMAC with sha-256 of data, as defined in
   * http://tools.ietf.org/html/rfc2104#section-2 .
   * @param key The key.
   * @param data The input byte array.
   * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to
   * receive the HMAC digest.
   */
  static void
  computeHmacWithSha256
    (const BlobLite& key, const BlobLite& data, uint8_t *digest)
  {
    computeHmacWithSha256(key.buf(), key.size(), data.buf(), data.size(), digest);
  }

  /**
   * Verify that the HMAC with sha-256 of the data equals the signature, as
   * defined in http://tools.ietf.org/html/rfc2104#section-2 . (This is a separate
   * method from computeHmacWithSha256 so that we can use a crypto-safe
   * comparison function.)
   * @param key A pointer to buffer with the key.
   * @param keyLength The length of key.
   * @param signature A pointer to the signature bytes.
   * @param signatureLength The length of signature.
   * @param data A pointer to the input byte array to verify.
   * @param dataLength The length of data.
   * @return True if the signature verifies, false if not.
   */
  static bool
  verifyHmacWithSha256Signature
    (const uint8_t *key, size_t keyLength, const uint8_t* signature,
     size_t signatureLength, const uint8_t *data, size_t dataLength);

  /**
   * Verify that the HMAC with sha-256 of the data equals the signature, as
   * defined in http://tools.ietf.org/html/rfc2104#section-2 . (This is a separate
   * method from computeHmacWithSha256 so that we can use a crypto-safe
   * comparison function.)
   * @param key The key.
   * @param signature The signature bytes.
   * @param data The byte array to verify.
   * @return True if the signature verifies, false if not.
   */
  static bool
  verifyHmacWithSha256Signature
    (const BlobLite& key, const BlobLite& signature, const BlobLite& data)
  {
    return verifyHmacWithSha256Signature
      (key.buf(), key.size(), signature.buf(), signature.size(), data.buf(),
       data.size());
  }

  /**
   * Verify that the DigestSha256 of the data equals the signature.
   * @param signature A pointer to the signature bytes.
   * @param signatureLength The length of signature.
   * @param data A pointer to the input byte array to verify.
   * @param dataLength The length of data.
   * @return True if the signature verifies, false if not.
   */
  static bool
  verifyDigestSha256Signature
    (const uint8_t* signature, size_t signatureLength, const uint8_t *data,
     size_t dataLength);

  /**
   * Verify that the DigestSha256 of the data equals the signature.
   * @param signature The signature bytes.
   * @param data The byte array to verify.
   * @return True if the signature verifies, false if not.
   */
  static bool
  verifyDigestSha256Signature(const BlobLite& signature, const BlobLite& data)
  {
    return verifyDigestSha256Signature
      (signature.buf(), signature.size(), data.buf(), data.size());
  }

  /**
   * Compute the PBKDF2 with HMAC SHA1 of the password.
   * @param password The input password, which should have characters in the range
   * of 1 to 127.
   * @param passwordLength The length of password.
   * @param salt The 8-byte salt.
   * @param saltLength The length of salt, which should be 8.
   * @param nIterations  The number of iterations of the hashing algorithm.
   * @param resultLength The number of bytes of the result array.
   * @param result A pointer to a buffer of size resultLength to receive the
   * result.
   */
  static void
  computePbkdf2WithHmacSha1
    (const uint8_t* password, size_t passwordLength, const uint8_t* salt,
     size_t saltLength, int nIterations, size_t resultLength, uint8_t* result);

  /**
   * Compute the PBKDF2 with HMAC SHA1 of the password.
   * @param password The input password, which should have characters in the range
   * of 1 to 127.
   * @param salt The 8-byte salt.
   * @param nIterations  The number of iterations of the hashing algorithm.
   * @param resultLength The number of bytes of the result array.
   * @param result A pointer to a buffer of size resultLength to receive the
   * result.
   */
  static void
  computePbkdf2WithHmacSha1
    (const BlobLite& password, const BlobLite& salt, int nIterations,
     size_t resultLength, uint8_t *result)
  {
    computePbkdf2WithHmacSha1
      (password.buf(), password.size(), salt.buf(), salt.size(), nIterations,
       resultLength, result);
  }

  /**
   * Compute the MurmurHash3 of the data.
   * @param nHashSeed The hash seed.
   * @param dataToHash A pointer to the input byte array to hash.
   * @param dataToHashLength The length of the data to hash.
   * @return The hash value.
   */
  static uint32_t
  murmurHash3
    (uint32_t nHashSeed, const uint8_t* dataToHash, size_t dataToHashLength);

  /**
   * Compute the MurmurHash3 of the data.
   * @param nHashSeed The hash seed.
   * @param dataToHash A pointer to the input byte array to hash.
   * @param dataToHashLength The length of the data to hash.
   * @return The hash value.
   */
  static uint32_t
  murmurHash3
    (uint32_t nHashSeed, const char* dataToHash, size_t dataToHashLength)
  {
    return murmurHash3(nHashSeed, (const uint8_t*)dataToHash, dataToHashLength);
  }

  /**
   * Compute the MurmurHash3 of the integer value.
   * @param nHashSeed The hash seed.
   * @param value The integer value, interpreted as a 4-byte array.
   * @return The hash value.
   */
  static uint32_t
  murmurHash3(uint32_t nHashSeed, uint32_t value);
};

}

#endif
