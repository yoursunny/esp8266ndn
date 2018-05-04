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

#include "../../forwarding-flags-impl.h"
#include "tlv-name.h"
#include "tlv-control-parameters.h"

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the Name
 * TLVs in the body of the ControlParameters strategy value.
 * @param context This is the ndn_ControlParameters struct pointer which
 * was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeStrategyValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_ControlParameters *controlParameters =
    (struct ndn_ControlParameters *)context;
  ndn_Error error;
  size_t dummyBeginOffset, dummyEndOffset;

  if ((error = ndn_encodeTlvName
       (&controlParameters->strategy, &dummyBeginOffset, &dummyEndOffset,
        encoder)))
    return error;

  return NDN_ERROR_success;
}

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs
 * in the body of the ControlParameters value.
 * @param context This is the ndn_ControlParameters struct pointer which
 * was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeControlParametersValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_ControlParameters *controlParameters =
    (struct ndn_ControlParameters *)context;
  ndn_Error error;
  size_t dummyBeginOffset, dummyEndOffset;
  struct ndn_ForwardingFlags defaultFlags;
  int flags;

  if (controlParameters->hasName) {
    if ((error = ndn_encodeTlvName
         (&controlParameters->name, &dummyBeginOffset, &dummyEndOffset,
          encoder)))
      return error;
  }
  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv
       (encoder, ndn_Tlv_ControlParameters_FaceId,
        controlParameters->faceId)))
    return error;
  if ((error = ndn_TlvEncoder_writeOptionalBlobTlv
       (encoder, ndn_Tlv_ControlParameters_Uri, &controlParameters->uri)))
    return error;
  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv
       (encoder, ndn_Tlv_ControlParameters_LocalControlFeature,
        controlParameters->localControlFeature)))
    return error;
  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv
       (encoder, ndn_Tlv_ControlParameters_Origin,
        controlParameters->origin)))
    return error;
  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv
       (encoder, ndn_Tlv_ControlParameters_Cost,
        controlParameters->cost)))
    return error;

  ndn_ForwardingFlags_initialize(&defaultFlags);
  flags = ndn_ForwardingFlags_getNfdForwardingFlags
    (&controlParameters->flags);
  if (flags != ndn_ForwardingFlags_getNfdForwardingFlags(&defaultFlags)) {
    // The flags are not the default value.
    if ((error = ndn_TlvEncoder_writeNonNegativeIntegerTlv
         (encoder, ndn_Tlv_ControlParameters_Flags, flags)))
      return error;
  }

  // Encode strategy.
  if (controlParameters->strategy.nComponents != 0) {
    if ((error = ndn_TlvEncoder_writeNestedTlv
         (encoder, ndn_Tlv_ControlParameters_Strategy,
          encodeStrategyValue, controlParameters, 0)))
      return error;
  }

  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlvFromDouble
       (encoder, ndn_Tlv_ControlParameters_ExpirationPeriod,
        controlParameters->expirationPeriod)))
    return error;

  return NDN_ERROR_success;
}

ndn_Error
ndn_encodeTlvControlParameters
  (const struct ndn_ControlParameters *controlParameters,
   struct ndn_TlvEncoder *encoder)
{
  return ndn_TlvEncoder_writeNestedTlv
    (encoder, ndn_Tlv_ControlParameters_ControlParameters,
     encodeControlParametersValue, controlParameters, 0);
}

ndn_Error
ndn_decodeTlvControlParameters
  (struct ndn_ControlParameters *controlParameters, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  int gotExpectedType;
  size_t dummyBeginOffset, dummyEndOffset;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart
       (decoder, ndn_Tlv_ControlParameters_ControlParameters, &endOffset)))
    return error;

  // Decode name.
  if ((error = ndn_TlvDecoder_peekType
       (decoder, ndn_Tlv_Name, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    controlParameters->hasName = 1;
    if ((error = ndn_decodeTlvName
         (&controlParameters->name, &dummyBeginOffset, &dummyEndOffset, decoder)))
      return error;
  }
  else {
    controlParameters->hasName = 0;
    controlParameters->name.nComponents = 0;
  }

  // Decode face ID.
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_ControlParameters_FaceId, endOffset,
        &controlParameters->faceId)))
    return error;

  // Decode URI.
  if ((error = ndn_TlvDecoder_readOptionalBlobTlv
       (decoder, ndn_Tlv_ControlParameters_Uri, endOffset,
        &controlParameters->uri)))
    return error;

  // Decode integers.
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_ControlParameters_LocalControlFeature, endOffset,
        &controlParameters->localControlFeature)))
    return error;
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_ControlParameters_Origin, endOffset,
        &controlParameters->origin)))
    return error;
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_ControlParameters_Cost, endOffset,
        &controlParameters->cost)))
    return error;

  // Set forwarding flags.
  if ((error = ndn_TlvDecoder_peekType
       (decoder, ndn_Tlv_ControlParameters_Flags, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    uint64_t flags;
    if ((error = ndn_TlvDecoder_readNonNegativeIntegerTlv
         (decoder, ndn_Tlv_ControlParameters_Flags, &flags)))
      return error;
    ndn_ForwardingFlags_setNfdForwardingFlags(&controlParameters->flags, (int)flags);
  }
  else
    ndn_ForwardingFlags_initialize(&controlParameters->flags);

  // Decode strategy.
  if ((error = ndn_TlvDecoder_peekType
       (decoder, ndn_Tlv_ControlParameters_Strategy, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    size_t strategyEndOffset;

    if ((error = ndn_TlvDecoder_readNestedTlvsStart
         (decoder, ndn_Tlv_ControlParameters_Strategy, &strategyEndOffset)))
      return error;
    if ((error = ndn_decodeTlvName
         (&controlParameters->strategy, &dummyBeginOffset, &dummyEndOffset,
          decoder)))
      return error;
    if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, strategyEndOffset)))
      return error;
  }
  else
    controlParameters->strategy.nComponents = 0;

  // Decode expiration period.
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlvAsDouble
       (decoder, ndn_Tlv_ControlParameters_ExpirationPeriod, endOffset,
        &controlParameters->expirationPeriod)))
    return error;

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
