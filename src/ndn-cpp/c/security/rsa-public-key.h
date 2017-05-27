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

#ifndef NDN_RSA_PUBLIC_KEY_H
#define NDN_RSA_PUBLIC_KEY_H

#include "../../c/common.h"
#include "../../c/errors.h"
#include "../../c/encrypt/algo/encrypt-params-types.h"
#include "../../c/security/rsa-public-key-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the ndn_RsaPublicKey struct with a null value.
 * @param self A pointer to the ndn_RsaPublicKey struct.
 */
static __inline void
ndn_RsaPublicKey_initialize(struct ndn_RsaPublicKey *self)
{
  self->publicKey = 0;
}

/**
 * Finalize the ndn_RsaPublicKey struct, freeing memory if needed.
 * @param self A pointer to the ndn_RsaPrivateKey struct.
 */
void
ndn_RsaPublicKey_finalize(struct ndn_RsaPublicKey *self);

/**
 * Decode the publicKeyDer and set the ndn_RsaPublicKey struct, allocating
 * memory as needed. You must call ndn_RsaPublicKey_finalize to free it.
 * @param self A pointer to the ndn_RsaPublicKey struct.
 * @param publicKeyDer A pointer to the DER-encoded public key.
 * @param publicKeyDerLength The length of publicKeyDer.
 * @return 0 for success, else NDN_ERROR_Error_decoding_key if publicKeyDer
 * can't be decoded as an RSA public key.
 */
ndn_Error
ndn_RsaPublicKey_decode
  (struct ndn_RsaPublicKey *self, const uint8_t *publicKeyDer,
   size_t publicKeyDerLength);

/**
 * Encode the DER-encoded SubjectPublicKeyInfo.
 * @param self A pointer to the ndn_RsaPublicKey struct.
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
ndn_RsaPublicKey_encode
  (const struct ndn_RsaPublicKey *self, uint8_t *encoding,
   size_t *encodingLength);

/**
 * Use the public key to verify the data using RsaWithSha256.
 * @param self A pointer to the ndn_RsaPublicKey struct.
 * @param signature A pointer to the signature bytes.
 * @param signatureLength The length of signature.
 * @param data A pointer to the input byte array to verify.
 * @param dataLength The length of data.
 * @return Nonzero if the signature verifies, 0 if not.
 */
int
ndn_RsaPublicKey_verifyWithSha256
  (const struct ndn_RsaPublicKey *self, const uint8_t *signature,
   size_t signatureLength, const uint8_t *data, size_t dataLength);

/**
 * Verify the RSA signature of the data using the given public key.
 * @param signature A pointer to the signature bytes.
 * @param signatureLength The length of signature.
 * @param data A pointer to the input byte array to verify.
 * @param dataLength The length of data.
 * @param publicKeyDer A pointer to the DER-encoded public key used to verify
 * the signature.
 * @param publicKeyDerLength The length of publicKeyDer.
 * @param verified Set verified to nonzero if the signature verifies, 0 if not.
 * @return 0 for success, else NDN_ERROR_Error_decoding_key if publicKeyDer
 * can't be decoded as an RSA public key.
 */
ndn_Error
ndn_verifySha256WithRsaSignature
  (const uint8_t *signature, size_t signatureLength, const uint8_t *data,
   size_t dataLength, const uint8_t *publicKeyDer, size_t publicKeyDerLength,
   int *verified);

/**
 * Use the public key to encrypt plainData according to the algorithmType.
 * @param self A pointer to the ndn_RsaPublicKey struct.
 * @param plainData A pointer to the input byte array to encrypt.
 * @param plainDataLength The length of plainData.
 * @param algorithmType This encrypts according to algorithmType.
 * @param encryptedData A pointer to the encrypted output buffer. The caller
 * must provide a buffer large enough to receive the bytes.
 * @param encryptedDataLength Set encryptedDataLength to the number of bytes
 * placed in the encryptedData buffer.
 * @return 0 for success, else NDN_ERROR_Unsupported_algorithm_type for
 * unsupported algorithmType padding scheme, or
 * NDN_ERROR_Error_in_encrypt_operation if can't complete the encrypt operation.
 */
ndn_Error
ndn_RsaPublicKey_encrypt
  (const struct ndn_RsaPublicKey *self, const uint8_t *plainData,
   size_t plainDataLength, ndn_EncryptAlgorithmType algorithmType,
   uint8_t *encryptedData, size_t *encryptedDataLength);

#ifdef __cplusplus
}
#endif

#endif
