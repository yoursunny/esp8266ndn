/**
 * Copyright (C) 2014-2018 Regents of the University of California.
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

#include <string.h>
#include "../../util/time.h"
#include "tlv-key-locator.h"
#include "tlv-signature-info.h"

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the
 * TLVs in the body of a signature's ValidityPeriod.
 * @param context This is the ndn_ValidityPeriod struct pointer which was passed
 * to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeValidityPeriodValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_ValidityPeriod *validityPeriod = (struct ndn_ValidityPeriod *)context;
  ndn_Error error;
  struct ndn_Blob isoStringBlob;
  char isoString[23];

  // Encode notBefore.
  if ((error = ndn_toIsoString(validityPeriod->notBefore, 0, isoString)))
    return error;
  ndn_Blob_initialize
    (&isoStringBlob, (const uint8_t *)isoString, strlen(isoString));
  if ((error = ndn_TlvEncoder_writeOptionalBlobTlv
       (encoder, ndn_Tlv_ValidityPeriod_NotBefore, &isoStringBlob)))
    return error;

  // Encode notAfter.
  if ((error = ndn_toIsoString(validityPeriod->notAfter, 0, isoString)))
    return error;
  ndn_Blob_initialize
    (&isoStringBlob, (const uint8_t *)isoString, strlen(isoString));
  if ((error = ndn_TlvEncoder_writeOptionalBlobTlv
       (encoder, ndn_Tlv_ValidityPeriod_NotAfter, &isoStringBlob)))
    return error;

  return NDN_ERROR_success;
}

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the
 * TLVs in the body of a signature value which has a KeyLocator and
 * ValidityPeriod, e.g. SignatureSha256WithRsa.
 * @param context This is the ndn_Signature struct pointer which was passed to
 * writeTlv. Use signature->type as the TLV type, assuming that the
 * ndn_SignatureType enum has the same values as the TLV signature types.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeSignatureWithKeyLocatorAndValidityPeriodValue
  (const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_Signature *signature = (struct ndn_Signature *)context;
  ndn_Error error;

  if ((error = ndn_TlvEncoder_writeNonNegativeIntegerTlv
       (encoder, ndn_Tlv_SignatureType, signature->type)))
    return error;
  if ((error = ndn_TlvEncoder_writeNestedTlv
       (encoder, ndn_Tlv_KeyLocator, ndn_encodeTlvKeyLocatorValue,
        &signature->keyLocator, 0)))
    return error;
  if (ndn_ValidityPeriod_hasPeriod(&signature->validityPeriod)) {
    if ((error = ndn_TlvEncoder_writeNestedTlv
         (encoder, ndn_Tlv_ValidityPeriod_ValidityPeriod,
          encodeValidityPeriodValue, &signature->validityPeriod, 0)))
      return error;
  }

  return NDN_ERROR_success;
}

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs
 * in the body of the DigestSha256 value.
 * @param context This is the ndn_Signature struct pointer which was passed to writeTlv.
 * (It is ignored.)
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeDigestSha256Value(const void *context, struct ndn_TlvEncoder *encoder)
{
  ndn_Error error;

  if ((error = ndn_TlvEncoder_writeNonNegativeIntegerTlv
       (encoder, ndn_Tlv_SignatureType, ndn_Tlv_SignatureType_DigestSha256)))
    return error;

  return NDN_ERROR_success;
}

static ndn_Error
decodeValidityPeriod
  (struct ndn_ValidityPeriod *validityPeriod, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  // Expect a 15-character ISO string like "20131018T184139".
  const size_t isoStringMaxLength = 15;
  char isoString[isoStringMaxLength + 1];
  struct ndn_Blob isoStringBlob;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart
       (decoder, ndn_Tlv_ValidityPeriod_ValidityPeriod, &endOffset)))
    return error;

  ndn_ValidityPeriod_clear(validityPeriod);

  // Decode notBefore as an ISO string.
  if ((error = ndn_TlvDecoder_readBlobTlv
       (decoder, ndn_Tlv_ValidityPeriod_NotBefore, &isoStringBlob)))
    return error;
  if (isoStringBlob.length > isoStringMaxLength)
    return NDN_ERROR_Calendar_time_value_out_of_range;
  ndn_memcpy((uint8_t *)isoString, isoStringBlob.value, isoStringBlob.length);
  isoString[isoStringBlob.length] = 0;
  if ((error = ndn_fromIsoString(isoString, &validityPeriod->notBefore)))
    return error;

  // Decode notAfter as an ISO string.
  if ((error = ndn_TlvDecoder_readBlobTlv
       (decoder, ndn_Tlv_ValidityPeriod_NotAfter, &isoStringBlob)))
    return error;
  if (isoStringBlob.length > isoStringMaxLength)
    return NDN_ERROR_Calendar_time_value_out_of_range;
  ndn_memcpy((uint8_t *)isoString, isoStringBlob.value, isoStringBlob.length);
  isoString[isoStringBlob.length] = 0;
  if ((error = ndn_fromIsoString(isoString, &validityPeriod->notAfter)))
    return error;

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}

ndn_Error
ndn_encodeTlvSignatureInfo
  (const struct ndn_Signature *signatureInfo, struct ndn_TlvEncoder *encoder)
{
  if (signatureInfo->type == ndn_SignatureType_Generic) {
    // Handle a Generic signature separately since it has the entire encoding.
    const struct ndn_Blob *encoding = &signatureInfo->signatureInfoEncoding;
    ndn_Error error;
    size_t endOffset;
    uint64_t signatureType;

    // Do a test decoding to sanity check that it is valid TLV.
    struct ndn_TlvDecoder decoder;
    ndn_TlvDecoder_initialize(&decoder, encoding->value, encoding->length);
    error = ndn_TlvDecoder_readNestedTlvsStart
      (&decoder, ndn_Tlv_SignatureInfo, &endOffset);
    if (!error)
      error = ndn_TlvDecoder_readNonNegativeIntegerTlv
        (&decoder, ndn_Tlv_SignatureType, &signatureType);
    if (!error)
      // Skip unrecognized TLVs, even if they have a critical type code.
      error = ndn_TlvDecoder_finishNestedTlvsSkipCritical(&decoder, endOffset);
    if (error)
      return NDN_ERROR_The_Generic_signature_encoding_is_not_a_valid_NDN_TLV_SignatureInfo;

    return ndn_TlvEncoder_writeArray(encoder, encoding->value, encoding->length);
  }

  if (signatureInfo->type == ndn_SignatureType_Sha256WithRsaSignature ||
      signatureInfo->type == ndn_SignatureType_Sha256WithEcdsaSignature ||
      signatureInfo->type == ndn_SignatureType_HmacWithSha256Signature)
    return ndn_TlvEncoder_writeNestedTlv
      (encoder, ndn_Tlv_SignatureInfo,
       encodeSignatureWithKeyLocatorAndValidityPeriodValue, signatureInfo, 0);
  else if (signatureInfo->type == ndn_SignatureType_DigestSha256Signature)
    return ndn_TlvEncoder_writeNestedTlv
      (encoder, ndn_Tlv_SignatureInfo, encodeDigestSha256Value,
       signatureInfo, 0);
  else
    return NDN_ERROR_encodeSignatureInfo_unrecognized_SignatureType;
}

ndn_Error
ndn_decodeTlvSignatureInfo
  (struct ndn_Signature *signatureInfo, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t beginOffset;
  size_t endOffset;
  uint64_t signatureType;
  int gotExpectedType;

  ndn_Signature_clear(signatureInfo);

  beginOffset = decoder->offset;
  if ((error = ndn_TlvDecoder_readNestedTlvsStart
       (decoder, ndn_Tlv_SignatureInfo, &endOffset)))
    return error;

  if ((error = ndn_TlvDecoder_readNonNegativeIntegerTlv
       (decoder, ndn_Tlv_SignatureType, &signatureType)))
    return error;

  if (signatureType == ndn_Tlv_SignatureType_SignatureSha256WithRsa ||
      signatureType == ndn_Tlv_SignatureType_SignatureSha256WithEcdsa ||
      signatureType == ndn_Tlv_SignatureType_SignatureHmacWithSha256) {
    // Assume that the ndn_SignatureType enum has the same values as the TLV
    // signature types.
    signatureInfo->type = signatureType;
    if ((error = ndn_decodeTlvKeyLocator
         (ndn_Tlv_KeyLocator, &signatureInfo->keyLocator, decoder)))
      return error;
    if ((error = ndn_TlvDecoder_peekType
         (decoder, ndn_Tlv_ValidityPeriod_ValidityPeriod, endOffset,
          &gotExpectedType)))
      return error;
    if (gotExpectedType) {
      if ((error = decodeValidityPeriod(&signatureInfo->validityPeriod, decoder)))
        return error;
    }
  }
  else if (signatureType == ndn_Tlv_SignatureType_DigestSha256)
    signatureInfo->type = ndn_SignatureType_DigestSha256Signature;
  else {
    signatureInfo->type = ndn_SignatureType_Generic;
    signatureInfo->genericTypeCode = signatureType;

    // Get the bytes of the SignatureInfo TLV.
    if ((error = ndn_TlvDecoder_getSlice
         (decoder, beginOffset, endOffset, &signatureInfo->signatureInfoEncoding)))
      return error;
    // Skip the remaining TLVs now, allowing unrecognized critical type codes.
    if ((error = ndn_TlvDecoder_finishNestedTlvsSkipCritical(decoder, endOffset)))
      return error;
  }

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
