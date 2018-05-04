/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#ifndef NDN_AES_ALGORITHM_LITE_HPP
#define NDN_AES_ALGORITHM_LITE_HPP

#include "../../util/blob-lite.hpp"
#include "../../../c/errors.h"

namespace ndn {

/**
 * The AesAlgorithmLite class provides static methods to encrypt and decrypt
 * using the AES symmetric key cipher.
 * @note This class is an experimental feature. The API may change.
 */
class AesAlgorithmLite {
public:
  /**
   * Use the key to decrypt encryptedData using AES 128 in CBC mode.
   * @param key A pointer to the key byte array.
   * @param keyLength The length of key. It is an error if this is not
   * ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check that the
   * correct algorithm is being used.
   * @param initialVector A pointer to the initial vector byte array.
   * @param initialVectorLength The length of initialVector. It is an error if
   * this is not ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check
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
  static ndn_Error
  decrypt128Cbc
    (const uint8_t* key, size_t keyLength, const uint8_t* initialVector,
     size_t initialVectorLength, const uint8_t* encryptedData,
     size_t encryptedDataLength, uint8_t* plainData, size_t& plainDataLength);

  /**
   * Use the key to decrypt encryptedData using AES 128 in CBC mode.
   * @param key The key byte array. It is an error if its size is not
   * ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check that the
   * correct algorithm is being used.
   * @param initialVector The initial vector byte array. It is an error if its
   * size is not ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check
   * that the correct algorithm is being used.
   * @param encryptedData The input byte array to decrypt.
   * @param plainData A pointer to the decrypted output buffer. The caller
   * must provide a large enough buffer, which should be at least
   * encryptedDataLength bytes.
   * @param plainDataLength This sets plainDataLength to the number of bytes
   * placed in the plainData buffer.
   * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
   * keyLength or NDN_ERROR_Incorrect_initial_vector_size for incorrect
   * initialVectorLength.
   */
  static ndn_Error
  decrypt128Cbc
    (const BlobLite& key, const BlobLite& initialVector,
     const BlobLite& encryptedData, uint8_t *plainData, size_t& plainDataLength)
  {
    return decrypt128Cbc
      (key.buf(), key.size(), initialVector.buf(), initialVector.size(),
       encryptedData.buf(), encryptedData.size(), plainData, plainDataLength);
  }

  /**
   * Use the key to decrypt encryptedData using AES 128 in ECB mode.
   * @param key A pointer to the key byte array.
   * @param keyLength The length of key. It is an error if this is not
   * ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check that the
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
  static ndn_Error
  decrypt128Ecb
    (const uint8_t* key, size_t keyLength, const uint8_t* encryptedData,
     size_t encryptedDataLength, uint8_t* plainData, size_t& plainDataLength);

  /**
   * Use the key to decrypt encryptedData using AES 128 in ECB mode.
   * @param key The key byte array. It is an error if its size is not
   * ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check that the
   * correct algorithm is being used.
   * @param encryptedData The input byte array to decrypt.
   * @param plainData A pointer to the decrypted output buffer. The caller
   * must provide a large enough buffer, which should be at least
   * encryptedDataLength bytes.
   * @param plainDataLength This sets plainDataLength to the number of bytes
   * placed in the plainData buffer.
   * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
   * keyLength.
   */
  static ndn_Error
  decrypt128Ecb
    (const BlobLite& key, const BlobLite& encryptedData, uint8_t *plainData,
     size_t& plainDataLength)
  {
    return decrypt128Ecb
      (key.buf(), key.size(), encryptedData.buf(), encryptedData.size(),
       plainData, plainDataLength);
  }

  /**
   * Use the key to encrypt encryptedData using AES 128 in CBC mode.
   * @param key A pointer to the key byte array.
   * @param keyLength The length of key. It is an error if this is not
   * ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check that the
   * correct algorithm is being used.
   * @param initialVector A pointer to the initial vector byte array.
   * @param initialVectorLength The length of initialVector. It is an error if
   * this is not ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check
   * that the correct algorithm is being used.
   * @param plainData A pointer to the input byte array to encrypt.
   * @param plainDataLength The length of plainData.
   * @param encryptedData A pointer to the decrypted output buffer. The caller
   * must provide a large enough buffer, which should be at least
   * encryptedDataLength + ndn_AES_128_BLOCK_SIZE bytes.
   * @param encryptedDataLength This sets encryptedDataLength to the number of
   * bytes placed in the encryptedData buffer.
   * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
   * keyLength or NDN_ERROR_Incorrect_initial_vector_size for incorrect
   * initialVectorLength.
   */
  static ndn_Error
  encrypt128Cbc
    (const uint8_t* key, size_t keyLength, const uint8_t* initialVector,
     size_t initialVectorLength, const uint8_t* plainData,
     size_t plainDataLength, uint8_t* encryptedData, size_t& encryptedDataLength);

  /**
   * Use the key to encrypt encryptedData using AES 128 in CBC mode.
   * @param key The key byte array. It is an error if its size is not
   * ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check that the
   * correct algorithm is being used.
   * @param initialVector The initial vector byte array. It is an error if its
   * size is not ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check
   * that the correct algorithm is being used.
   * @param plainData The input byte array to encrypt.
   * @param encryptedData A pointer to the decrypted output buffer. The caller
   * must provide a large enough buffer, which should be at least
   * encryptedDataLength + ndn_AES_128_BLOCK_SIZE bytes.
   * @param encryptedDataLength This sets encryptedDataLength to the number of
   * bytes placed in the encryptedData buffer.
   * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
   * keyLength or NDN_ERROR_Incorrect_initial_vector_size for incorrect
   * initialVectorLength.
   */
  static ndn_Error
  encrypt128Cbc
    (const BlobLite& key, const BlobLite& initialVector,
     const BlobLite& plainData, uint8_t *encryptedData, size_t& encryptedDataLength)
  {
    return encrypt128Cbc
      (key.buf(), key.size(), initialVector.buf(), initialVector.size(),
       plainData.buf(), plainData.size(), encryptedData, encryptedDataLength);
  }

/**
 * Use the key to encrypt encryptedData using AES 128 in ECB mode.
 * @param key A pointer to the key byte array.
 * @param keyLength The length of key. It is an error if this is not
 * ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check that the
 * correct algorithm is being used.
 * @param plainData A pointer to the input byte array to encrypt.
 * @param plainDataLength The length of plainData.
 * @param encryptedData A pointer to the decrypted output buffer. The caller
 * must provide a large enough buffer, which should be at least
 * encryptedDataLength + ndn_AES_128_BLOCK_SIZE bytes.
 * @param encryptedDataLength This sets encryptedDataLength to the number of
 * bytes placed in the encryptedData buffer.
 * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
 * keyLength.
 */
  static ndn_Error
  encrypt128Ecb
    (const uint8_t* key, size_t keyLength, const uint8_t* plainData,
     size_t plainDataLength, uint8_t* encryptedData, size_t& encryptedDataLength);

  /**
   * Use the key to encrypt encryptedData using AES 128 in ECB mode.
   * @param key The key byte array. It is an error if its size is not
   * ndn_AES_128_BLOCK_SIZE. This value is proved as a safety check that the
   * correct algorithm is being used.
   * @param plainData The input byte array to encrypt.
   * @param encryptedData A pointer to the decrypted output buffer. The caller
   * must provide a large enough buffer, which should be at least
   * encryptedDataLength + ndn_AES_128_BLOCK_SIZE bytes.
   * @param encryptedDataLength This sets encryptedDataLength to the number of
   * bytes placed in the encryptedData buffer.
   * @return 0 for success, else NDN_ERROR_Incorrect_key_size for incorrect
   * keyLength.
   */
  static ndn_Error
  encrypt128Ecb
    (const BlobLite& key, const BlobLite& plainData, uint8_t *encryptedData,
     size_t& encryptedDataLength)
  {
    return encrypt128Ecb
      (key.buf(), key.size(), plainData.buf(), plainData.size(), encryptedData,
       encryptedDataLength);
  }
};

}

#endif
