/**
 * Copyright (C) 2016-2017 Regents of the University of California.
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

#ifndef NDN_RSA_PRIVATE_KEY_H
#define NDN_RSA_PRIVATE_KEY_H

#include "../../c/common.h"
#include "../../c/errors.h"
#include "../../c/encrypt/algo/encrypt-params-types.h"
#include "../../c/security/rsa-private-key-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the ndn_RsaPrivateKey struct with a null value.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 */
static __inline void
ndn_RsaPrivateKey_initialize(struct ndn_RsaPrivateKey *self)
{
  self->privateKey = 0;
}

/**
 * Finalize the ndn_RsaPrivateKey struct, freeing memory if needed.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 */
void
ndn_RsaPrivateKey_finalize(struct ndn_RsaPrivateKey *self);

/**
 * Decode the DER-encoded PKCS #1 privateKeyDer and set the ndn_RsaPrivateKey
 * struct, allocating memory as needed. You must call
 * ndn_RsaPrivateKey_finalize to free it.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 * @param privateKeyDer A pointer to the DER-encoded private key.
 * @param privateKeyDerLength The length of privateKeyDer.
 * @return 0 for success, else NDN_ERROR_Error_decoding_key if privateKeyDer
 * can't be decoded as an RSA private key.
 */
ndn_Error
ndn_RsaPrivateKey_decode
  (struct ndn_RsaPrivateKey *self, const uint8_t *privateKeyDer,
   size_t privateKeyDerLength);

/**
 * Generate a key pair and set the ndn_RsaPrivateKey struct, allocating
 * memory as needed. You must call ndn_RsaPrivateKey_finalize to free it.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 * @param keySize The size in bits of the key to generate.
 * @return 0 for success, else NDN_ERROR_Error_in_generate_operation if can't
 * complete the generate operation.
 */
ndn_Error
ndn_RsaPrivateKey_generate(struct ndn_RsaPrivateKey *self, uint32_t keySize);

/**
 * Encode the DER-encoded PKCS #1 private key.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 * @param encoding A pointer to the encoding output buffer. If this is null then
 * only set encodingLength (which can be used to allocate a buffer of the
 * correct size). Otherwise, the caller must provide a buffer large enough to
 * receive the encoding bytes.
 * @param encodingLength Set encodingLength to the number of bytes in the
 * encoding.
 * @return 0 for success, else NDN_ERROR_Error_encoding_key if can't encode the
 * key.
 */
ndn_Error
ndn_RsaPrivateKey_encodePrivateKey
  (const struct ndn_RsaPrivateKey *self, uint8_t *encoding,
   size_t *encodingLength);

/**
 * Encode the DER-encoded SubjectPublicKeyInfo.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 * @param encoding A pointer to the encoding output buffer. If this is null then
 * only set encodingLength (which can be used to allocate a buffer of the
 * correct size). Otherwise, the caller must provide a buffer large enough to
 * receive the encoding bytes.
 * @param encodingLength Set encodingLength to the number of bytes in the
 * encoding.
 * @return 0 for success, else NDN_ERROR_Error_encoding_key if can't encode the
 * key.
 */
ndn_Error
ndn_RsaPrivateKey_encodePublicKey
  (const struct ndn_RsaPrivateKey *self, uint8_t *encoding,
   size_t *encodingLength);

/**
 * Use the private key to sign the data using RsaWithSha256.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 * @param data A pointer to the input byte array to sign.
 * @param dataLength The length of data.
 * @param signature A pointer to the signature output buffer. The caller must
 * provide a buffer large enough to receive the signature bytes.
 * @param signatureLength Set signatureLength to the number of bytes placed in
 * the signature buffer.
 * @return 0 for success, else NDN_ERROR_Error_in_sign_operation if can't
 * complete the sign operation.
 */
ndn_Error
ndn_RsaPrivateKey_signWithSha256
  (const struct ndn_RsaPrivateKey *self, const uint8_t *data, size_t dataLength,
   uint8_t *signature, size_t *signatureLength);

/**
 * Use the private key to decrypt encryptedData according to the algorithmType.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 * @param encryptedData A pointer to the input byte array to decrypt.
 * @param encryptedDataLength The length of encryptedData.
 * @param algorithmType This decrypts according to algorithmType.
 * @param plainData A pointer to the decrypted output buffer. The caller
 * must provide a buffer large enough to receive the bytes.
 * @param plainDataLength Set plainDataLength to the number of bytes placed in
 * the plainData buffer.
 * @return 0 for success, else NDN_ERROR_Unsupported_algorithm_type for
 * unsupported algorithmType padding scheme, or
 * NDN_ERROR_Error_in_decrypt_operation if can't complete the decrypt operation.
 */
ndn_Error
ndn_RsaPrivateKey_decrypt
  (const struct ndn_RsaPrivateKey *self, const uint8_t *encryptedData,
   size_t encryptedDataLength, ndn_EncryptAlgorithmType algorithmType,
   uint8_t *plainData, size_t *plainDataLength);

#ifdef __cplusplus
}
#endif

#endif
