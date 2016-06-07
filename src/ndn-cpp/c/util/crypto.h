/**
 * Copyright (C) 2013-2016 Regents of the University of California.
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

#ifndef NDN_CRYPTO_H
#define NDN_CRYPTO_H

#include <ndn-cpp/c/common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The ndn_EcKeyInfo struct has fields to relate EC key size, curve IDs and OIDs.
 */
struct ndn_EcKeyInfo {
  int keySize;
  int curveId;
  int *oidIntegerList;
  size_t oidIntegerListLength;
};

/**
 * Compute the sha-256 digest of data.
 * @param data Pointer to the input byte array.
 * @param dataLength The length of data.
 * @param digest A pointer to a buffer of size ndn_SHA256_DIGEST_SIZE to receive
 * the digest.
 */
void ndn_digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest);

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
void
ndn_computeHmacWithSha256
  (const uint8_t *key, size_t keyLength, const uint8_t *data, size_t dataLength,
   uint8_t *digest);

/**
 * Fill the buffer with random bytes.
 * @param buffer Write the random bytes to this buffer.
 * @param bufferLength The number of bytes to write to buffer.
 */
void
ndn_generateRandomBytes(uint8_t *buffer, size_t bufferLength);

/**
 * Get the number of ndn_EcKeyInfo struct entries in the array.
 * @return The number of entries.
 */
size_t
ndn_getEcKeyInfoCount();

/**
 * Get the ndn_EcKeyInfo at the given index.
 * @param i The index into the array of ndn_EcKeyInfo struct, from zero to
 * ndn_getEcKeyInfoCount() - 1.
 * @return A pointer to the ndn_EcKeyInfo struct.
 */
const struct ndn_EcKeyInfo *
ndn_getEcKeyInfo(int i);

#ifdef __cplusplus
}
#endif

#endif
