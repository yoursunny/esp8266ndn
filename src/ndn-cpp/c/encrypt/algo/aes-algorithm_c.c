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

#include "../../util/crypto.h"
#include "aes-algorithm.h"

#if NDN_CPP_HAVE_LIBCRYPTO

#include <openssl/evp.h>

ndn_Error
ndn_AesAlgorithm_decrypt128Cbc
  (const uint8_t *key, size_t keyLength, const uint8_t *initialVector,
   size_t initialVectorLength, const uint8_t *encryptedData,
   size_t encryptedDataLength, uint8_t *plainData, size_t *plainDataLength)
{
  EVP_CIPHER_CTX *ctx;
  int outLength1, outLength2;

  if (keyLength != ndn_AES_128_KEY_LENGTH)
    return NDN_ERROR_Incorrect_key_size;
  if (initialVectorLength != ndn_AES_BLOCK_LENGTH)
    return NDN_ERROR_Incorrect_initial_vector_size;

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return NDN_ERROR_Error_in_decrypt_operation;

  EVP_DecryptInit
    (ctx, EVP_aes_128_cbc(), (const unsigned char*)key,
     (const unsigned char*)initialVector);

  EVP_DecryptUpdate
    (ctx, (unsigned char*)plainData, &outLength1,
     (const unsigned char*)encryptedData, encryptedDataLength);
  EVP_DecryptFinal
    (ctx, (unsigned char*)plainData + outLength1, &outLength2);

  EVP_CIPHER_CTX_free(ctx);
  *plainDataLength = outLength1 + outLength2;

  return NDN_ERROR_success;
}

ndn_Error
ndn_AesAlgorithm_decrypt256Cbc
  (const uint8_t *key, size_t keyLength, const uint8_t *initialVector,
   size_t initialVectorLength, const uint8_t *encryptedData,
   size_t encryptedDataLength, uint8_t *plainData, size_t *plainDataLength)
{
  EVP_CIPHER_CTX *ctx;
  int outLength1, outLength2;

  if (keyLength != ndn_AES_256_KEY_LENGTH)
    return NDN_ERROR_Incorrect_key_size;
  if (initialVectorLength != ndn_AES_BLOCK_LENGTH)
    return NDN_ERROR_Incorrect_initial_vector_size;

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return NDN_ERROR_Error_in_decrypt_operation;

  EVP_DecryptInit
    (ctx, EVP_aes_256_cbc(), (const unsigned char*)key,
     (const unsigned char*)initialVector);

  EVP_DecryptUpdate
    (ctx, (unsigned char*)plainData, &outLength1,
     (const unsigned char*)encryptedData, encryptedDataLength);
  EVP_DecryptFinal
    (ctx, (unsigned char*)plainData + outLength1, &outLength2);

  EVP_CIPHER_CTX_free(ctx);
  *plainDataLength = outLength1 + outLength2;

  return NDN_ERROR_success;
}

ndn_Error
ndn_AesAlgorithm_decrypt128Ecb
  (const uint8_t *key, size_t keyLength, const uint8_t *encryptedData,
   size_t encryptedDataLength, uint8_t *plainData, size_t *plainDataLength)
{
  EVP_CIPHER_CTX *ctx;
  int outLength1, outLength2;

  if (keyLength != ndn_AES_128_KEY_LENGTH)
    return NDN_ERROR_Incorrect_key_size;

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return NDN_ERROR_Error_in_decrypt_operation;

  EVP_DecryptInit(ctx, EVP_aes_128_ecb(), (const unsigned char*)key, 0);

  EVP_DecryptUpdate
    (ctx, (unsigned char*)plainData, &outLength1,
     (const unsigned char*)encryptedData, encryptedDataLength);
  EVP_DecryptFinal
    (ctx, (unsigned char*)plainData + outLength1, &outLength2);

  EVP_CIPHER_CTX_free(ctx);
  *plainDataLength = outLength1 + outLength2;

  return NDN_ERROR_success;
}

ndn_Error
ndn_AesAlgorithm_encrypt128Cbc
  (const uint8_t *key, size_t keyLength, const uint8_t *initialVector,
   size_t initialVectorLength, const uint8_t *plainData,
   size_t plainDataLength, uint8_t *encryptedData, size_t *encryptedDataLength)
{
  EVP_CIPHER_CTX *ctx;
  int outLength1, outLength2;

  if (keyLength != ndn_AES_128_KEY_LENGTH)
    return NDN_ERROR_Incorrect_key_size;
  if (initialVectorLength != ndn_AES_BLOCK_LENGTH)
    return NDN_ERROR_Incorrect_initial_vector_size;

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return NDN_ERROR_Error_in_encrypt_operation;

  EVP_EncryptInit
      (ctx, EVP_aes_128_cbc(), (const unsigned char*)key,
       (const unsigned char*)initialVector);

  EVP_EncryptUpdate
    (ctx, (unsigned char*)encryptedData, &outLength1,
     (const unsigned char*)plainData, plainDataLength);
  EVP_EncryptFinal
    (ctx, (unsigned char*)encryptedData + outLength1, &outLength2);

  EVP_CIPHER_CTX_free(ctx);
  *encryptedDataLength = outLength1 + outLength2;

  return NDN_ERROR_success;
}

ndn_Error
ndn_AesAlgorithm_encrypt256Cbc
  (const uint8_t *key, size_t keyLength, const uint8_t *initialVector,
   size_t initialVectorLength, const uint8_t *plainData,
   size_t plainDataLength, uint8_t *encryptedData, size_t *encryptedDataLength)
{
  EVP_CIPHER_CTX *ctx;
  int outLength1, outLength2;

  if (keyLength != ndn_AES_256_KEY_LENGTH)
    return NDN_ERROR_Incorrect_key_size;
  if (initialVectorLength != ndn_AES_BLOCK_LENGTH)
    return NDN_ERROR_Incorrect_initial_vector_size;

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return NDN_ERROR_Error_in_encrypt_operation;

  EVP_EncryptInit
      (ctx, EVP_aes_256_cbc(), (const unsigned char*)key,
       (const unsigned char*)initialVector);

  EVP_EncryptUpdate
    (ctx, (unsigned char*)encryptedData, &outLength1,
     (const unsigned char*)plainData, plainDataLength);
  EVP_EncryptFinal
    (ctx, (unsigned char*)encryptedData + outLength1, &outLength2);

  EVP_CIPHER_CTX_free(ctx);
  *encryptedDataLength = outLength1 + outLength2;

  return NDN_ERROR_success;
}

ndn_Error
ndn_AesAlgorithm_encrypt128Ecb
  (const uint8_t *key, size_t keyLength, const uint8_t *plainData,
   size_t plainDataLength, uint8_t *encryptedData, size_t *encryptedDataLength)
{
  EVP_CIPHER_CTX *ctx;
  int outLength1, outLength2;

  if (keyLength != ndn_AES_128_KEY_LENGTH)
    return NDN_ERROR_Incorrect_key_size;

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    return NDN_ERROR_Error_in_encrypt_operation;

  EVP_EncryptInit(ctx, EVP_aes_128_ecb(), (const unsigned char*)key, 0);

  EVP_EncryptUpdate
    (ctx, (unsigned char*)encryptedData, &outLength1,
     (const unsigned char*)plainData, plainDataLength);
  EVP_EncryptFinal
    (ctx, (unsigned char*)encryptedData + outLength1, &outLength2);

  EVP_CIPHER_CTX_free(ctx);
  *encryptedDataLength = outLength1 + outLength2;

  return NDN_ERROR_success;
}

#endif // NDN_CPP_HAVE_LIBCRYPTO
