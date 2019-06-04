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

#ifndef NDN_AES_ALGORITHM_H
#define NDN_AES_ALGORITHM_H

#include "../../../c/common.h"
#include "../../../c/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Use the key to decrypt encryptedData using AES 128 in CBC mode.
 * @param key A pointer to the key byte array.
 * @param keyLength The length of key. It is an error if this is not
 * ndn_AES_128_KEY_LENGTH. This value is proved as a safety check that the
 * correct algorithm is being used.
 * @param initialVector A pointer to the initial vector byte array.
 * @param initialVectorLength The length of initialVector. It is an error if
 * this is not ndn_AES_BLOCK_LENGTH. This value is proved as a safety check
 * that the correct algorithm is being used.
 * @param encryptedData A pointer to the input byte array to decrypt.
 * @param encryptedDataLength The length of encryptedData.
 * @param plainData A pointer to the decrypted output buffer. The caller
 * must provide a large enough buffer, which should be at least
 * encryptedDataLength bytes.
 * @param plainDataLength This sets plainDataLength to the number of bytes
 * placed in the plainData buffer.
 * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
 * keyLength or NDN_ERROR_Incorrect_initial_vector_size for incorrect
 * initialVectorLength.
 */
ndn_Error
ndn_AesAlgorithm_decrypt128Cbc
  (const uint8_t *key, size_t keyLength, const uint8_t *initialVector,
   size_t initialVectorLength, const uint8_t *encryptedData,
   size_t encryptedDataLength, uint8_t *plainData, size_t *plainDataLength);

/**
 * Use the key to decrypt encryptedData using AES 256 in CBC mode.
 * @param key A pointer to the key byte array.
 * @param keyLength The length of key. It is an error if this is not
 * ndn_AES_256_KEY_LENGTH. This value is proved as a safety check that the
 * correct algorithm is being used.
 * @param initialVector A pointer to the initial vector byte array.
 * @param initialVectorLength The length of initialVector. It is an error if
 * this is not ndn_AES_BLOCK_LENGTH. This value is proved as a safety check
 * that the correct algorithm is being used.
 * @param encryptedData A pointer to the input byte array to decrypt.
 * @param encryptedDataLength The length of encryptedData.
 * @param plainData A pointer to the decrypted output buffer. The caller
 * must provide a large enough buffer, which should be at least
 * encryptedDataLength bytes.
 * @param plainDataLength This sets plainDataLength to the number of bytes
 * placed in the plainData buffer.
 * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
 * keyLength or NDN_ERROR_Incorrect_initial_vector_size for incorrect
 * initialVectorLength.
 */
ndn_Error
ndn_AesAlgorithm_decrypt256Cbc
  (const uint8_t *key, size_t keyLength, const uint8_t *initialVector,
   size_t initialVectorLength, const uint8_t *encryptedData,
   size_t encryptedDataLength, uint8_t *plainData, size_t *plainDataLength);

/**
 * Use the key to decrypt encryptedData using AES 128 in ECB mode.
 * @param key A pointer to the key byte array.
 * @param keyLength The length of key. It is an error if this is not
 * ndn_AES_128_KEY_LENGTH. This value is proved as a safety check that the
 * correct algorithm is being used.
 * @param encryptedData A pointer to the input byte array to decrypt.
 * @param encryptedDataLength The length of encryptedData.
 * @param plainData A pointer to the decrypted output buffer. The caller
 * must provide a large enough buffer, which should be at least
 * encryptedDataLength bytes.
 * @param plainDataLength This sets plainDataLength to the number of bytes
 * placed in the plainData buffer.
 * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
 * keyLength.
 */
ndn_Error
ndn_AesAlgorithm_decrypt128Ecb
  (const uint8_t *key, size_t keyLength, const uint8_t *encryptedData,
   size_t encryptedDataLength, uint8_t *plainData, size_t *plainDataLength);

/**
 * Use the key to encrypt encryptedData using AES 128 in CBC mode.
 * @param key A pointer to the key byte array.
 * @param keyLength The length of key. It is an error if this is not
 * ndn_AES_128_KEY_LENGTH. This value is proved as a safety check that the
 * correct algorithm is being used.
 * @param initialVector A pointer to the initial vector byte array.
 * @param initialVectorLength The length of initialVector. It is an error if
 * this is not ndn_AES_BLOCK_LENGTH. This value is proved as a safety check
 * that the correct algorithm is being used.
 * @param plainData A pointer to the input byte array to encrypt.
 * @param plainDataLength The length of plainData.
 * @param encryptedData A pointer to the decrypted output buffer. The caller
 * must provide a large enough buffer, which should be at least
 * encryptedDataLength + ndn_AES_BLOCK_LENGTH bytes.
 * @param encryptedDataLength This sets encryptedDataLength to the number of
 * bytes placed in the encryptedData buffer.
 * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
 * keyLength or NDN_ERROR_Incorrect_initial_vector_size for incorrect
 * initialVectorLength.
 */
ndn_Error
ndn_AesAlgorithm_encrypt128Cbc
  (const uint8_t *key, size_t keyLength, const uint8_t *initialVector,
   size_t initialVectorLength, const uint8_t *plainData,
   size_t plainDataLength, uint8_t *encryptedData, size_t *encryptedDataLength);

/**
 * Use the key to encrypt encryptedData using AES 256 in CBC mode.
 * @param key A pointer to the key byte array.
 * @param keyLength The length of key. It is an error if this is not
 * ndn_AES_256_KEY_LENGTH. This value is proved as a safety check that the
 * correct algorithm is being used.
 * @param initialVector A pointer to the initial vector byte array.
 * @param initialVectorLength The length of initialVector. It is an error if
 * this is not ndn_AES_BLOCK_LENGTH. This value is proved as a safety check
 * that the correct algorithm is being used.
 * @param plainData A pointer to the input byte array to encrypt.
 * @param plainDataLength The length of plainData.
 * @param encryptedData A pointer to the decrypted output buffer. The caller
 * must provide a large enough buffer, which should be at least
 * encryptedDataLength + ndn_AES_BLOCK_LENGTH bytes.
 * @param encryptedDataLength This sets encryptedDataLength to the number of
 * bytes placed in the encryptedData buffer.
 * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
 * keyLength or NDN_ERROR_Incorrect_initial_vector_size for incorrect
 * initialVectorLength.
 */
ndn_Error
ndn_AesAlgorithm_encrypt256Cbc
  (const uint8_t *key, size_t keyLength, const uint8_t *initialVector,
   size_t initialVectorLength, const uint8_t *plainData,
   size_t plainDataLength, uint8_t *encryptedData, size_t *encryptedDataLength);

/**
 * Use the key to encrypt encryptedData using AES 128 in ECB mode.
 * @param key A pointer to the key byte array.
 * @param keyLength The length of key. It is an error if this is not
 * ndn_AES_128_KEY_LENGTH. This value is proved as a safety check that the
 * correct algorithm is being used.
 * @param plainData A pointer to the input byte array to encrypt.
 * @param plainDataLength The length of plainData.
 * @param encryptedData A pointer to the decrypted output buffer. The caller
 * must provide a large enough buffer, which should be at least
 * encryptedDataLength + ndn_AES_BLOCK_LENGTH bytes.
 * @param encryptedDataLength This sets encryptedDataLength to the number of
 * bytes placed in the encryptedData buffer.
 * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
 * keyLength.
 */
ndn_Error
ndn_AesAlgorithm_encrypt128Ecb
  (const uint8_t *key, size_t keyLength, const uint8_t *plainData,
   size_t plainDataLength, uint8_t *encryptedData, size_t *encryptedDataLength);

#ifdef __cplusplus
}
#endif

#endif
