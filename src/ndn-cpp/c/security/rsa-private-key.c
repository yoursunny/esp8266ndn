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
#include "rsa-private-key.h"

#if NDN_CPP_HAVE_LIBCRYPTO

#include <openssl/ssl.h>

void
ndn_RsaPrivateKey_finalize(struct ndn_RsaPrivateKey *self)
{
  if (self->privateKey) {
    RSA_free(self->privateKey);
    self->privateKey = 0;
  }
}

ndn_Error
ndn_RsaPrivateKey_decode
  (struct ndn_RsaPrivateKey *self, const uint8_t *privateKeyDer,
   size_t privateKeyDerLength)
{
  if (self->privateKey)
    // Free a previous value.
    RSA_free(self->privateKey);

  self->privateKey = d2i_RSAPrivateKey(NULL, &privateKeyDer, privateKeyDerLength);
  if (!self->privateKey)
    return NDN_ERROR_Error_decoding_key;
  return NDN_ERROR_success;
}

ndn_Error
ndn_RsaPrivateKey_generate(struct ndn_RsaPrivateKey *self, uint32_t keySize)
{
  BIGNUM* exponent = 0;
  int success = 0;

  if (self->privateKey) {
    // Free a previous value.
    RSA_free(self->privateKey);
    self->privateKey = 0;
  }

  exponent = BN_new();
  if (BN_set_word(exponent, RSA_F4) == 1) {
    self->privateKey = RSA_new();
    if (RSA_generate_key_ex(self->privateKey, keySize, exponent, NULL) == 1)
      success = 1;
  }

  BN_free(exponent);
  if (success)
    return NDN_ERROR_success;
  else {
    RSA_free(self->privateKey);
    self->privateKey = 0;
    return NDN_ERROR_Error_in_generate_operation;
  }
}

ndn_Error
ndn_RsaPrivateKey_encodePrivateKey
  (const struct ndn_RsaPrivateKey *self, uint8_t *encoding,
   size_t *encodingLength)
{
  int result = i2d_RSAPrivateKey(self->privateKey, encoding ? &encoding : 0);
  if (result < 0)
    return NDN_ERROR_Error_encoding_key;

  *encodingLength = result;
  return NDN_ERROR_success;
}

ndn_Error
ndn_RsaPrivateKey_encodePublicKey
  (const struct ndn_RsaPrivateKey *self, uint8_t *encoding,
   size_t *encodingLength)
{
  int result = i2d_RSA_PUBKEY(self->privateKey, encoding ? &encoding : 0);
  if (result < 0)
    return NDN_ERROR_Error_encoding_key;

  *encodingLength = result;
  return NDN_ERROR_success;
}

ndn_Error
ndn_RsaPrivateKey_signWithSha256
  (const struct ndn_RsaPrivateKey *self, const uint8_t *data, size_t dataLength,
   uint8_t *signature, size_t *signatureLength)
{
  // Make a temporary length variable of the correct type.
  unsigned int tempSignatureLength;
  uint8_t digest[ndn_SHA256_DIGEST_SIZE];
  ndn_digestSha256(data, dataLength, digest);

  if (!RSA_sign(NID_sha256, digest, sizeof(digest), (unsigned char *)signature,
                &tempSignatureLength, self->privateKey))
    return NDN_ERROR_Error_in_sign_operation;

  *signatureLength = tempSignatureLength;
  return NDN_ERROR_success;
}

ndn_Error
ndn_RsaPrivateKey_decrypt
  (const struct ndn_RsaPrivateKey *self, const uint8_t *encryptedData,
   size_t encryptedDataLength, ndn_EncryptAlgorithmType algorithmType,
   uint8_t *plainData, size_t *plainDataLength)
{
  int padding;
  int outputLength;

  if (algorithmType == ndn_EncryptAlgorithmType_RsaPkcs)
    padding = RSA_PKCS1_PADDING;
  else if (algorithmType == ndn_EncryptAlgorithmType_RsaOaep)
    padding = RSA_PKCS1_OAEP_PADDING;
  else
    return NDN_ERROR_Unsupported_algorithm_type;

  outputLength = RSA_private_decrypt
    ((int)encryptedDataLength, (unsigned char *)encryptedData,
     (unsigned char*)plainData, self->privateKey, padding);

  if (outputLength < 0)
    return NDN_ERROR_Error_in_decrypt_operation;

  *plainDataLength = outputLength;
  return NDN_ERROR_success;
}

#endif // NDN_CPP_HAVE_LIBCRYPTO
