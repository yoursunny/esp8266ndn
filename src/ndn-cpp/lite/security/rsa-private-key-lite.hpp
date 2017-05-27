/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
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

#ifndef NDN_RSA_PRIVATE_KEY_LITE_HPP
#define NDN_RSA_PRIVATE_KEY_LITE_HPP

#include "../util/blob-lite.hpp"
#include "../../c/errors.h"
#include "../../c/encrypt/algo/encrypt-params-types.h"
#include "../../c/security/rsa-private-key-types.h"

namespace ndn {

/**
 * An RsaPrivateKeyLite holds a decoded or generated RSA private key for use in
 * crypto operations.
 */
class RsaPrivateKeyLite : private ndn_RsaPrivateKey {
public:
  /**
   * Create an RsaPrivateKeyLite with a null value.
   */
  RsaPrivateKeyLite();

  /**
   * Finalize the RsaPrivateKeyLite, freeing memory if needed.
   */
  ~RsaPrivateKeyLite();

  /**
   * Decode the DER-encoded PKCS #1 privateKeyDer and set this RsaPrivateKeyLite,
   * allocating memory as needed.
   * @param privateKeyDer A pointer to the DER-encoded private key.
   * @param privateKeyDerLength The length of privateKeyDer.
   * @return 0 for success, else NDN_ERROR_Error_decoding_key if privateKeyDer
   * can't be decoded as an RSA private key.
   */
  ndn_Error
  decode(const uint8_t* privateKeyDer, size_t privateKeyDerLength);

  /**
   * Decode the DER-encoded PKCS #1 privateKeyDer and set this RsaPrivateKeyLite,
   * allocating memory as needed.
   * @param privateKeyDer The DER-encoded private key.
   * @return 0 for success, else NDN_ERROR_Error_decoding_key if privateKeyDer
   * can't be decoded as an RSA private key.
   */
  ndn_Error
  decode(const BlobLite& privateKeyDer)
  {
    return decode(privateKeyDer.buf(), privateKeyDer.size());
  }

  /**
   * Generate a key pair and set this RsaPrivateKeyLite, allocating memory as
   * needed.
   * @param keySize The size in bits of the key to generate.
   * @return 0 for success, else NDN_ERROR_Error_in_generate_operation if can't
   * complete the generate operation.
   */
  ndn_Error
  generate(uint32_t keySize);

  /**
   * Encode the DER-encoded PKCS #1 private key.
   * @param encoding A pointer to the encoding output buffer. If this is null
   * then only set encodingLength (which can be used to allocate a buffer of the
   * correct size). Otherwise, the caller must provide a buffer large enough to
   * receive the encoding bytes.
   * @param encodingLength Set encodingLength to the number of bytes in the
   * encoding.
   * @return 0 for success, else NDN_ERROR_Error_encoding_key if can't encode the
   * key.
   */
  ndn_Error
  encodePrivateKey(uint8_t* encoding, size_t& encodingLength) const;

  /**
   * Encode the DER-encoded SubjectPublicKeyInfo.
   * @param encoding A pointer to the encoding output buffer. If this is null
   * then only set encodingLength (which can be used to allocate a buffer of the
   * correct size). Otherwise, the caller must provide a buffer large enough to
   * receive the encoding bytes.
   * @param encodingLength Set encodingLength to the number of bytes in the
   * encoding.
   * @return 0 for success, else NDN_ERROR_Error_encoding_key if can't encode the
   * key.
   */
  ndn_Error
  encodePublicKey(uint8_t* encoding, size_t& encodingLength) const;

  /**
   * Use this private key to sign the data using RsaWithSha256.
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
  signWithSha256
    (const uint8_t* data, size_t dataLength, uint8_t* signature,
     size_t& signatureLength) const;

  /**
   * Use this private key to sign the data using RsaWithSha256.
   * @param data The input byte array to sign.
   * @param signature A pointer to the signature output buffer. The caller must
   * provide a buffer large enough to receive the signature bytes.
   * @param signatureLength Set signatureLength to the number of bytes placed in
   * the signature buffer.
   * @return 0 for success, else NDN_ERROR_Error_in_sign_operation if can't
   * complete the sign operation.
   */
  ndn_Error
  signWithSha256
    (const BlobLite& data, uint8_t* signature, size_t& signatureLength) const
  {
    return signWithSha256(data.buf(), data.size(), signature, signatureLength);
  }

  /**
   * Use the private key to decrypt encryptedData according to the algorithmType.
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
  decrypt
    (const uint8_t* encryptedData, size_t encryptedDataLength,
     ndn_EncryptAlgorithmType algorithmType, uint8_t* plainData,
     size_t& plainDataLength);

  /**
   * Use the private key to decrypt encryptedData according to the algorithmType.
   * @param encryptedData The input byte array to decrypt.
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
  decrypt
    (const BlobLite& encryptedData, ndn_EncryptAlgorithmType algorithmType,
     uint8_t* plainData, size_t& plainDataLength)
  {
    return decrypt
      (encryptedData.buf(), encryptedData.size(), algorithmType, plainData,
       plainDataLength);
  }

  /**
   * Downcast the reference to the ndn_RsaPrivateKey struct to a RsaPrivateKeyLite.
   * @param blob A reference to the ndn_RsaPrivateKey struct.
   * @return The same reference as RsaPrivateKeyLite.
   */
  static RsaPrivateKeyLite&
  downCast(ndn_RsaPrivateKey& blob) { return *(RsaPrivateKeyLite*)&blob; }

  static const RsaPrivateKeyLite&
  downCast(const ndn_RsaPrivateKey& blob) { return *(RsaPrivateKeyLite*)&blob; }

private:
  // Don't allow copying since we don't reference count the allocated value.
  RsaPrivateKeyLite(const RsaPrivateKeyLite& other);
  RsaPrivateKeyLite& operator=(const RsaPrivateKeyLite& other);
};

}

#endif
