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

#ifndef NDN_EC_PRIVATE_KEY_H
#define NDN_EC_PRIVATE_KEY_H

#include "../../c/common.h"
#include "../../c/errors.h"
#include "../../c/security/ec-private-key-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the ndn_EcPrivateKey struct with a null value.
 * @param self A pointer to the ndn_EcPrivateKey struct.
 */
static __inline void
ndn_EcPrivateKey_initialize(struct ndn_EcPrivateKey *self)
{
  self->privateKey = 0;
}

/**
 * Finalize the ndn_EcPrivateKey struct, freeing memory if needed.
 * @param self A pointer to the ndn_EcPrivateKey struct.
 */
void
ndn_EcPrivateKey_finalize(struct ndn_EcPrivateKey *self);

/**
 * Decode the privateKeyDer and set the ndn_EcPrivateKey struct, allocating
 * memory as needed. You must call ndn_EcPrivateKey_finalize to free it.
 * @param self A pointer to the ndn_EcPrivateKey struct.
 * @param privateKeyDer A pointer to the DER-encoded private key.
 * @param privateKeyDerLength The length of privateKeyDer.
 * @return 0 for success, else NDN_ERROR_Error_decoding_key if privateKeyDer
 * can't be decoded as an EC private key.
 */
ndn_Error
ndn_EcPrivateKey_decode
  (struct ndn_EcPrivateKey *self, const uint8_t *privateKeyDer,
   size_t privateKeyDerLength);

/**
 * Set the the private key from the given curveId, using the value to create a
 * BIGNUM, allocating memory as needed. You must call ndn_EcPrivateKey_finalize
 * to free it.
 * @param self A pointer to the ndn_EcPrivateKey struct.
 * @param curveId The OpenSSL curve ID such as NID_secp384r1.
 * @param value A pointer to the value array for the BIGNUM.
 * @param valueLength The length of value.
 * @return 0 for success, else NDN_ERROR_Error_decoding_key if can't create
 * the private key from the curveId or value.
 */
ndn_Error
ndn_EcPrivateKey_setByCurve
  (struct ndn_EcPrivateKey *self, int curveId, const uint8_t *value,
   size_t valueLength);

/**
 * Generate a key pair and set the ndn_EcPrivateKey struct, allocating
 * memory as needed. You must call ndn_EcPrivateKey_finalize to free it.
 * @param self A pointer to the ndn_EcPrivateKey struct.
 * @param keySize The size in bits of the key to generate.
 * @return 0 for success, else NDN_ERROR_Error_in_generate_operation if can't
 * complete the generate operation, including if a curve can't be found for the
 * keySize.
 */
ndn_Error
ndn_EcPrivateKey_generate(struct ndn_EcPrivateKey *self, uint32_t keySize);

/**
 * Encode the DER-encoded private key.
 * @param self A pointer to the ndn_EcPrivateKey struct.
 * @param includeParameters If nonzero, then include the EC parameters in the
 * encoding.
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
ndn_EcPrivateKey_encodePrivateKey
  (const struct ndn_EcPrivateKey *self, int includeParameters, uint8_t *encoding,
   size_t *encodingLength);

/**
 * Encode the DER-encoded EC SubjectPublicKeyInfo.
 * @param self A pointer to the ndn_EcPrivateKey struct.
 * @param includeParameters If nonzero, then include the EC parameters in the
 * encoding.
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
ndn_EcPrivateKey_encodePublicKey
  (const struct ndn_EcPrivateKey *self, int includeParameters, uint8_t *encoding,
   size_t *encodingLength);

/**
 * Use the private key to sign the data using EcdsaWithSha256.
 * @param self A pointer to the ndn_EcPrivateKey struct.
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
ndn_EcPrivateKey_signWithSha256
  (const struct ndn_EcPrivateKey *self, const uint8_t *data, size_t dataLength,
   uint8_t *signature, size_t *signatureLength);

/**
 * Get the OpenSSL curve ID.
 * @param self A pointer to the ndn_EcPrivateKey struct.
 * @param curveId Set curveId to the OpenSSL curve ID such as NID_secp384r1.
 * @return 0 for success, else NDN_ERROR_Error_decoding_key if can't get the
 * curve ID.
 */
ndn_Error
ndn_EcPrivateKey_getCurveId(const struct ndn_EcPrivateKey *self, int *curveId);

#ifdef __cplusplus
}
#endif

#endif
