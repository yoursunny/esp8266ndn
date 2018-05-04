/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#include "tlv-control-parameters.h"
#include "tlv-control-response.h"

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs
 * in the body of the ControlResponse value.
 * @param context This is the ndn_ControlResponse struct pointer which
 * was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeControlResponseValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_ControlResponse *controlResponse =
    (struct ndn_ControlResponse *)context;
  ndn_Error error;
  struct ndn_ForwardingFlags defaultFlags;

  if ((error = ndn_TlvEncoder_writeNonNegativeIntegerTlv
       (encoder, ndn_Tlv_NfdCommand_StatusCode,
        controlResponse->statusCode)))
    return error;
  if ((error = ndn_TlvEncoder_writeBlobTlv
       (encoder, ndn_Tlv_NfdCommand_StatusText, &controlResponse->statusText)))
    return error;
  if (controlResponse->hasBodyAsControlParameters) {
    if ((error = ndn_encodeTlvControlParameters
         (&controlResponse->bodyAsControlParameters, encoder)))
      return error;
  }

  return NDN_ERROR_success;
}

ndn_Error
ndn_encodeTlvControlResponse
  (const struct ndn_ControlResponse *controlResponse,
   struct ndn_TlvEncoder *encoder)
{
  return ndn_TlvEncoder_writeNestedTlv
    (encoder, ndn_Tlv_NfdCommand_ControlResponse,
     encodeControlResponseValue, controlResponse, 0);
}

ndn_Error
ndn_decodeTlvControlResponse
  (struct ndn_ControlResponse *controlResponse, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  int gotExpectedType;
  uint64_t statusCode;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart
       (decoder, ndn_Tlv_NfdCommand_ControlResponse, &endOffset)))
    return error;

  if ((error = ndn_TlvDecoder_readNonNegativeIntegerTlv
       (decoder, ndn_Tlv_NfdCommand_StatusCode, &statusCode)))
    return error;
  controlResponse->statusCode = (int)statusCode;
  if ((error = ndn_TlvDecoder_readBlobTlv
       (decoder, ndn_Tlv_NfdCommand_StatusText, &controlResponse->statusText)))
    return error;

  // Decode the body.
  if ((error = ndn_TlvDecoder_peekType
       (decoder, ndn_Tlv_ControlParameters_ControlParameters, endOffset,
        &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    controlResponse->hasBodyAsControlParameters = 1;
    if ((error = ndn_decodeTlvControlParameters
         (&controlResponse->bodyAsControlParameters, decoder)))
      return error;
  }
  else
    controlResponse->hasBodyAsControlParameters = 0;

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
