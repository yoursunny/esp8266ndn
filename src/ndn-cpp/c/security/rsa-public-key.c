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

#include "../util/crypto.h"
#include "rsa-public-key.h"

#if NDN_CPP_HAVE_LIBCRYPTO

#include <openssl/ssl.h>

void
ndn_RsaPublicKey_finalize(struct ndn_RsaPublicKey *self)
{
  if (self->publicKey) {
    RSA_free(self->publicKey);
    self->publicKey = 0;
  }
}

ndn_Error
ndn_RsaPublicKey_decode
  (struct ndn_RsaPublicKey *self, const uint8_t *publicKeyDer,
   size_t publicKeyDerLength)
{
  if (self->publicKey)
    // Free a previous value.
    RSA_free(self->publicKey);

  self->publicKey = d2i_RSA_PUBKEY(NULL, &publicKeyDer, publicKeyDerLength);
  if (!self->publicKey)
    return NDN_ERROR_Error_decoding_key;
  return NDN_ERROR_success;
}

ndn_Error
ndn_RsaPublicKey_encode
  (const struct ndn_RsaPublicKey *self, uint8_t *encoding,
   size_t *encodingLength)
{
  int result = i2d_RSA_PUBKEY(self->publicKey, encoding ? &encoding : 0);
  if (result < 0)
    return NDN_ERROR_Error_encoding_key;

  *encodingLength = result;
  return NDN_ERROR_success;
}

int
ndn_RsaPublicKey_verifyWithSha256
  (const struct ndn_RsaPublicKey *self, const uint8_t *signature,
   size_t signatureLength, const uint8_t *data, size_t dataLength)
{
  // Set digest to the digest of the signed portion of the signedBlob.
  uint8_t digest[ndn_SHA256_DIGEST_SIZE];
  ndn_digestSha256(data, dataLength, digest);
  return RSA_verify
    (NID_sha256, digest, sizeof(digest), (uint8_t *)signature, signatureLength,
     self->publicKey) == 1;
}

ndn_Error
ndn_verifySha256WithRsaSignature
  (const uint8_t *signature, size_t signatureLength, const uint8_t *data,
   size_t dataLength, const uint8_t *publicKeyDer, size_t publicKeyDerLength,
   int *verified)
{
  ndn_Error error;

  struct ndn_RsaPublicKey publicKey;
  ndn_RsaPublicKey_initialize(&publicKey);

  if ((error = ndn_RsaPublicKey_decode(&publicKey, publicKeyDer, publicKeyDerLength)))
    return error;

  *verified = ndn_RsaPublicKey_verifyWithSha256
    (&publicKey, signature, signatureLength, data, dataLength);
  ndn_RsaPublicKey_finalize(&publicKey);
  return NDN_ERROR_success;
}

ndn_Error
ndn_RsaPublicKey_encrypt
  (const struct ndn_RsaPublicKey *self, const uint8_t *plainData,
   size_t plainDataLength, ndn_EncryptAlgorithmType algorithmType,
   uint8_t *encryptedData, size_t *encryptedDataLength)
{
  int padding;
  int outputLength;

  if (algorithmType == ndn_EncryptAlgorithmType_RsaPkcs)
    padding = RSA_PKCS1_PADDING;
  else if (algorithmType == ndn_EncryptAlgorithmType_RsaOaep)
    padding = RSA_PKCS1_OAEP_PADDING;
  else
    return NDN_ERROR_Unsupported_algorithm_type;

  outputLength = RSA_public_encrypt
    ((int)plainDataLength, (unsigned char *)plainData,
     (unsigned char*)encryptedData, self->publicKey, padding);

  if (outputLength < 0)
    return NDN_ERROR_Error_in_encrypt_operation;

  *encryptedDataLength = outputLength;
  return NDN_ERROR_success;
}

#endif // NDN_CPP_HAVE_LIBCRYPTO
