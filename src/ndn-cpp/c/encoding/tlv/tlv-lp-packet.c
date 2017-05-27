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

#include "../../network-nack.h"
#include "../../lp/incoming-face-id.h"
#include "tlv-lp-packet.h"

ndn_Error
ndn_decodeTlvLpPacket
  (struct ndn_LpPacket *lpPacket, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;

  ndn_LpPacket_clear(lpPacket);

  if ((error = ndn_TlvDecoder_readNestedTlvsStart
       (decoder, ndn_Tlv_LpPacket_LpPacket, &endOffset)))
    return error;

  while (decoder->offset < endOffset) {
    // Imitate ndn_TlvDecoder_readTypeAndLength.
    uint64_t fieldTypeVarNumber;
    int fieldType;
    uint64_t fieldLength;
    size_t fieldEndOffset;

    if ((error = ndn_TlvDecoder_readVarNumber(decoder, &fieldTypeVarNumber)))
      return error;
    fieldType = (int)fieldTypeVarNumber;
    if ((error = ndn_TlvDecoder_readVarNumber(decoder, &fieldLength)))
      return error;
    fieldEndOffset = decoder->offset + (size_t)fieldLength;
    if (fieldEndOffset > decoder->inputLength)
      return NDN_ERROR_TLV_length_exceeds_buffer_length;

    if (fieldType == ndn_Tlv_LpPacket_Fragment) {
      // Set the fragment to the bytes of the TLV value.
      if ((error = ndn_TlvDecoder_getSlice
           (decoder, decoder->offset, fieldEndOffset,
            &lpPacket->fragmentWireEncoding)))
        return error;
      ndn_TlvDecoder_seek(decoder, fieldEndOffset);

      // The fragment is supposed to be the last field.
      break;
    }
    else if (fieldType == ndn_Tlv_LpPacket_Nack) {
      struct ndn_NetworkNack *networkNack;
      int code;

      if ((error = ndn_NetworkNack_add(lpPacket, &networkNack)))
        return error;
      if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
           (decoder, ndn_Tlv_LpPacket_NackReason, fieldEndOffset, &code)))
        return error;
      // The enum numeric values are the same as this wire format, so use as is.
      if (code < 0 || code == ndn_NetworkNackReason_NONE)
        // This includes an omitted NackReason.
        networkNack->reason = ndn_NetworkNackReason_NONE;
      else if (code == ndn_NetworkNackReason_CONGESTION ||
               code == ndn_NetworkNackReason_DUPLICATE ||
               code == ndn_NetworkNackReason_NO_ROUTE)
        networkNack->reason = code;
      else {
        // Unrecognized reason.
        networkNack->reason = ndn_NetworkNackReason_OTHER_CODE;
        networkNack->otherReasonCode = code;
      }
    }
    else if (fieldType == ndn_Tlv_LpPacket_IncomingFaceId) {
      struct ndn_IncomingFaceId *incomingFaceId;

      if ((error = ndn_IncomingFaceId_add(lpPacket, &incomingFaceId)))
        return error;
      if ((error = ndn_TlvDecoder_readNonNegativeInteger
           (decoder, fieldLength, &incomingFaceId->faceId)))
        return error;
    }
    else {
      // Unrecognized field type. The conditions for ignoring are here:
      // http://redmine.named-data.net/projects/nfd/wiki/NDNLPv2
      int canIgnore =
        (fieldType >= ndn_Tlv_LpPacket_IGNORE_MIN &&
         fieldType <= ndn_Tlv_LpPacket_IGNORE_MAX &&
         (fieldType & 0x01) == 1);
      if (!canIgnore)
        return NDN_ERROR_did_not_get_the_expected_TLV_type;

      // Ignore.
      ndn_TlvDecoder_seek(decoder, fieldEndOffset);
    }

    if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, fieldEndOffset)))
      return error;
  }

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
