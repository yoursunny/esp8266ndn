/**
 * Copyright (C) 2014-2016 Regents of the University of California.
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

#include "tlv-key-locator.h"
#include "tlv-signature-info.h"

/**
 * This private function is called by ndn_TlvEncoder_writeTlv to write the TLVs
 * in the body of a signature value which has a KeyLocator, e.g.
 * SignatureSha256WithRsa.
 * @param context This is the ndn_Signature struct pointer which was passed to
 * writeTlv. Use signature->type as the TLV type, assuming that the
 * ndn_SignatureType enum has the same values as the TLV signature types.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeSignatureWithKeyLocatorValue
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

  return NDN_ERROR_success;
}

/**
 * This private function is called by ndn_TlvEncoder_writeTlv to write the TLVs
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
      error = ndn_TlvDecoder_finishNestedTlvs(&decoder, endOffset);
    if (error)
      return NDN_ERROR_The_Generic_signature_encoding_is_not_a_valid_NDN_TLV_SignatureInfo;

    return ndn_TlvEncoder_writeArray(encoder, encoding->value, encoding->length);
  }

  if (signatureInfo->type == ndn_SignatureType_Sha256WithRsaSignature ||
      signatureInfo->type == ndn_SignatureType_Sha256WithEcdsaSignature ||
      signatureInfo->type == ndn_SignatureType_HmacWithSha256Signature)
    return ndn_TlvEncoder_writeNestedTlv
      (encoder, ndn_Tlv_SignatureInfo, encodeSignatureWithKeyLocatorValue,
       signatureInfo, 0);
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
  }

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
