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

#ifndef NDN_RSA_PUBLIC_KEY_LITE_HPP
#define NDN_RSA_PUBLIC_KEY_LITE_HPP

#include "../util/blob-lite.hpp"
#include "../../c/errors.h"
#include "../../c/encrypt/algo/encrypt-params-types.h"
#include "../../c/security/rsa-public-key-types.h"

namespace ndn {

/**
 * An RsaPublicKeyLite holds a decoded RSA public key for use in crypto
 * operations.
 */
class RsaPublicKeyLite : private ndn_RsaPublicKey {
public:
  /**
   * Create an RsaPublicKeyLite with a null value.
   */
  RsaPublicKeyLite();

  /**
   * Finalize the RsaPublicKeyLite, freeing memory if needed.
   */
  ~RsaPublicKeyLite();

  /**
   * Decode the publicKeyDer and set this RsaPublicKeyLite, allocating
   * memory as needed.
   * @param publicKeyDer A pointer to the DER-encoded public key.
   * @param publicKeyDerLength The length of publicKeyDer.
   * @return 0 for success, else NDN_ERROR_Error_decoding_key if publicKeyDer
   * can't be decoded as an RSA public key.
   */
  ndn_Error
  decode(const uint8_t* publicKeyDer, size_t publicKeyDerLength);

  /**
   * Decode the publicKeyDer and set this RsaPublicKeyLite, allocating
   * memory as needed.
   * @param publicKeyDer The DER-encoded public key.
   * @return 0 for success, else NDN_ERROR_Error_decoding_key if publicKeyDer
   * can't be decoded as an RSA public key.
   */
  ndn_Error
  decode(const BlobLite& publicKeyDer)
  {
    return decode(publicKeyDer.buf(), publicKeyDer.size());
  }

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
  encode(uint8_t* encoding, size_t& encodingLength) const;

  /**
   * Use this public key to verify the data using RsaWithSha256.
   * @param signature A pointer to the signature bytes.
   * @param signatureLength The length of signature.
   * @param data A pointer to the input byte array to verify.
   * @param dataLength The length of data.
   * @return True if the signature verifies, false if not.
   */
  bool
  verifyWithSha256
    (const uint8_t* signature, size_t signatureLength, const uint8_t* data,
     size_t dataLength) const;

  /**
   * Use this public key to verify the data using RsaWithSha256.
   * @param signature The signature bytes.
   * @param data The input byte array to verify.
   * @return True if the signature verifies, false if not.
   */
  bool
  verifyWithSha256(const BlobLite& signature, const BlobLite& data) const
  {
    return verifyWithSha256
      (signature.buf(), signature.size(), data.buf(), data.size());
  }

  /**
   * Verify the RSA signature of the data using the given public key.
   * @param signature A pointer to the signature bytes.
   * @param signatureLength The length of signature.
   * @param data A pointer to the input byte array to verify.
   * @param dataLength The length of data.
   * @param publicKeyDer A pointer to the DER-encoded public key used to verify
   * the signature.
   * @param publicKeyDerLength The length of publicKeyDer.
   * @param verified Set verified to true if the signature verifies, false if not.
   * @return 0 for success, else NDN_ERROR_Error_decoding_key if publicKeyDer
   * can't be decoded as an RSA public key.
   */
  static ndn_Error
  verifySha256WithRsaSignature
    (const uint8_t *signature, size_t signatureLength, const uint8_t *data,
     size_t dataLength, const uint8_t *publicKeyDer, size_t publicKeyDerLength,
     bool &verified);

  /**
   * Verify the RSA signature of the data using the given public key.
   * @param signature The signature bytes.
   * @param data The input byte array to verify.
   * @param publicKeyDer The DER-encoded public key used to verify the signature.
   * @param verified Set verified to true if the signature verifies, false if not.
   * @return 0 for success, else NDN_ERROR_Error_decoding_key if publicKeyDer
   * can't be decoded as an RSA public key.
   */
  static ndn_Error
  verifySha256WithRsaSignature
    (const BlobLite& signature, const BlobLite& data,
     const BlobLite& publicKeyDer, bool &verified)
  {
    return verifySha256WithRsaSignature
      (signature.buf(), signature.size(), data.buf(), data.size(),
       publicKeyDer.buf(), publicKeyDer.size(), verified);
  }

  /**
   * Use this public key to encrypt plainData according to the algorithmType.
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
  encrypt
    (const uint8_t* plainData, size_t plainDataLength,
     ndn_EncryptAlgorithmType algorithmType, uint8_t* encryptedData,
     size_t& encryptedDataLength) const;

  /**
   * Use this public key to encrypt plainData according to the algorithmType.
   * @param plainData The input byte array to encrypt.
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
  encrypt
    (const BlobLite& plainData, ndn_EncryptAlgorithmType algorithmType,
     uint8_t* encryptedData, size_t& encryptedDataLength) const
  {
    return encrypt
      (plainData.buf(), plainData.size(), algorithmType, encryptedData,
       encryptedDataLength);
  }

  /**
   * Downcast the reference to the ndn_RsaPublicKey struct to a RsaPublicKeyLite.
   * @param blob A reference to the ndn_RsaPublicKey struct.
   * @return The same reference as RsaPublicKeyLite.
   */
  static RsaPublicKeyLite&
  downCast(ndn_RsaPublicKey& blob) { return *(RsaPublicKeyLite*)&blob; }

  static const RsaPublicKeyLite&
  downCast(const ndn_RsaPublicKey& blob) { return *(RsaPublicKeyLite*)&blob; }

private:
  // Don't allow copying since we don't reference count the allocated value.
  RsaPublicKeyLite(const RsaPublicKeyLite& other);
  RsaPublicKeyLite& operator=(const RsaPublicKeyLite& other);
};

}

#endif
