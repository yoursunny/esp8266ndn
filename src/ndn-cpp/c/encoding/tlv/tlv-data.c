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

#include "tlv-name.h"
#include "tlv-key-locator.h"
#include "tlv-signature-info.h"
#include "tlv-data.h"

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs in the body of the MetaInfo value.
 * @param context This is the ndn_MetaInfo struct pointer which was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeMetaInfoValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_MetaInfo *metaInfo = (struct ndn_MetaInfo *)context;

  ndn_Error error;

  if (!((int)metaInfo->type < 0 || metaInfo->type == ndn_ContentType_BLOB)) {
    // Not the default, so we need to encode the type.
    if (metaInfo->type == ndn_ContentType_LINK ||
        metaInfo->type == ndn_ContentType_KEY ||
        metaInfo->type == ndn_ContentType_NACK) {
      // The ContentType enum is set up with the correct integer for each NDN-TLV ContentType.
      if ((error = ndn_TlvEncoder_writeNonNegativeIntegerTlv
          (encoder, ndn_Tlv_ContentType, metaInfo->type)))
        return error;
    }
    else if (metaInfo->type == ndn_ContentType_OTHER_CODE) {
      if ((error = ndn_TlvEncoder_writeNonNegativeIntegerTlv
          (encoder, ndn_Tlv_ContentType, metaInfo->otherTypeCode)))
        return error;
    }
    else
      // We don't expect this to happen.
      return NDN_ERROR_unrecognized_ndn_ContentType;
  }

  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlvFromDouble
      (encoder, ndn_Tlv_FreshnessPeriod, metaInfo->freshnessPeriod)))
    return error;
  if (metaInfo->finalBlockId.value.value &&
      metaInfo->finalBlockId.value.length > 0) {
    // The FinalBlockId has an inner NameComponent.
    if ((error = ndn_TlvEncoder_writeTypeAndLength
         (encoder, ndn_Tlv_FinalBlockId, ndn_TlvEncoder_sizeOfBlobTlv
            (metaInfo->finalBlockId.type, &metaInfo->finalBlockId.value))))
      return error;
    if ((error = ndn_encodeTlvNameComponent(&metaInfo->finalBlockId, encoder)))
      return error;
  }

  return NDN_ERROR_success;
}

/* An DataValueContext is for passing the context to encodeDataValue so that we can include
 * signedPortionBeginOffset and signedPortionEndOffset.
 */
struct DataValueContext {
  const struct ndn_Data *data;
  size_t *signedPortionBeginOffset;
  size_t *signedPortionEndOffset;
};

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs in the body of the Data value.
 * @param context This is the DataValueContext struct pointer which was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeDataValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  const struct DataValueContext *dataValueContext = (const struct DataValueContext *)context;
  const struct ndn_Data *data = dataValueContext->data;
  ndn_Error error;
  size_t dummyBeginOffset, dummyEndOffset;

  *dataValueContext->signedPortionBeginOffset = encoder->offset;

  if ((error = ndn_encodeTlvName
       (&data->name, &dummyBeginOffset, &dummyEndOffset, encoder)))
    return error;
  if ((error = ndn_TlvEncoder_writeNestedTlv(encoder, ndn_Tlv_MetaInfo, encodeMetaInfoValue, &data->metaInfo, 0)))
    return error;
  if ((error = ndn_TlvEncoder_writeBlobTlv(encoder, ndn_Tlv_Content, &data->content)))
    return error;
  if ((error = ndn_encodeTlvSignatureInfo(&data->signature, encoder)))
    return error;

  *dataValueContext->signedPortionEndOffset = encoder->offset;

  if ((error = ndn_TlvEncoder_writeBlobTlv(encoder, ndn_Tlv_SignatureValue, &data->signature.signature)))
    return error;

  return NDN_ERROR_success;
}

ndn_Error
ndn_encodeTlvData
  (const struct ndn_Data *data, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_TlvEncoder *encoder)
{
  // Create the context to pass to encodeDataValue.
  struct DataValueContext dataValueContext;
  dataValueContext.data = data;
  dataValueContext.signedPortionBeginOffset = signedPortionBeginOffset;
  dataValueContext.signedPortionEndOffset = signedPortionEndOffset;

  return ndn_TlvEncoder_writeNestedTlv(encoder, ndn_Tlv_Data, encodeDataValue, &dataValueContext, 0);
}

static ndn_Error
decodeMetaInfo(struct ndn_MetaInfo *metaInfo, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  int gotExpectedType;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, ndn_Tlv_MetaInfo, &endOffset)))
    return error;

  int type;
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_ContentType, endOffset, &type)))
    return error;
  if (type < 0 || type == ndn_ContentType_BLOB)
    // Default to BLOB if the value is omitted.
    metaInfo->type = ndn_ContentType_BLOB;
  else if (type == ndn_ContentType_LINK ||
           type == ndn_ContentType_KEY ||
           type == ndn_ContentType_NACK)
    // The ContentType enum is set up with the correct integer for each NDN-TLV ContentType.
    metaInfo->type = type;
  else {
    // Unrecognized content type.
    metaInfo->type = ndn_ContentType_OTHER_CODE;
    metaInfo->otherTypeCode = type;
  }

  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlvAsDouble
       (decoder, ndn_Tlv_FreshnessPeriod, endOffset, &metaInfo->freshnessPeriod)))
    return error;

  if ((error = ndn_TlvDecoder_peekType
       (decoder, ndn_Tlv_FinalBlockId, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    size_t finalBlockIdEndOffset;
    if ((error = ndn_TlvDecoder_readNestedTlvsStart
         (decoder, ndn_Tlv_FinalBlockId, &finalBlockIdEndOffset)))
      return error;
    if ((error = ndn_decodeTlvNameComponent(&metaInfo->finalBlockId, decoder)))
      return error;
    if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, finalBlockIdEndOffset)))
      return error;
  }
  else
    ndn_NameComponent_initialize(&metaInfo->finalBlockId, 0, 0);

  // Set fields not used by NDN-TLV to none.
  metaInfo->timestampMilliseconds = -1;

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}

ndn_Error
ndn_decodeTlvData
  (struct ndn_Data *data, size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  size_t dummyBeginOffset, dummyEndOffset;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, ndn_Tlv_Data, &endOffset)))
    return error;

  *signedPortionBeginOffset = decoder->offset;

  if ((error = ndn_decodeTlvName
       (&data->name, &dummyBeginOffset, &dummyEndOffset, decoder)))
    return error;
  if ((error = decodeMetaInfo(&data->metaInfo, decoder)))
    return error;
  if ((error = ndn_TlvDecoder_readBlobTlv(decoder, ndn_Tlv_Content, &data->content)))
    return error;
  if ((error = ndn_decodeTlvSignatureInfo(&data->signature, decoder)))
    return error;

  *signedPortionEndOffset = decoder->offset;

  if ((error = ndn_TlvDecoder_readBlobTlv(decoder, ndn_Tlv_SignatureValue, &data->signature.signature)))
    return error;

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
