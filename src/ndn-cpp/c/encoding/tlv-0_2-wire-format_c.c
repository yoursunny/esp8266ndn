/**
 * Copyright (C) 2015-2018 Regents of the University of California.
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

#include "tlv/tlv-name.h"
#include "tlv/tlv-interest.h"
#include "tlv/tlv-data.h"
#include "tlv/tlv-control-parameters.h"
#include "tlv/tlv-control-response.h"
#include "tlv/tlv-signature-info.h"
#include "tlv/tlv-lp-packet.h"
#include "tlv/tlv-delegation-set.h"
#include "tlv/tlv-encrypted-content.h"
#include "tlv-0_2-wire-format.h"

ndn_Error
ndn_Tlv0_2WireFormat_encodeName
  (const struct ndn_Name *name, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_DynamicUInt8Array *output,
   size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  error = ndn_encodeTlvName
    (name, signedPortionBeginOffset, signedPortionEndOffset, &encoder);
  *encodingLength = encoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeName
  (struct ndn_Name *name, const uint8_t *input, size_t inputLength,
   size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset)
{
  struct ndn_TlvDecoder decoder;
  ndn_TlvDecoder_initialize(&decoder, input, inputLength);
  return ndn_decodeTlvName
    (name, signedPortionBeginOffset, signedPortionEndOffset, &decoder);
}

ndn_Error
ndn_Tlv0_2WireFormat_encodeInterest
  (const struct ndn_Interest *interest, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_DynamicUInt8Array *output,
   size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  error = ndn_encodeTlvInterest
    (interest, signedPortionBeginOffset, signedPortionEndOffset, &encoder);
  *encodingLength = encoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeInterest
  (struct ndn_Interest *interest, const uint8_t *input, size_t inputLength,
   size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset)
{
  struct ndn_TlvDecoder decoder;
  ndn_TlvDecoder_initialize(&decoder, input, inputLength);
  return ndn_decodeTlvInterest
    (interest, signedPortionBeginOffset, signedPortionEndOffset, &decoder);
}

ndn_Error
ndn_Tlv0_2WireFormat_encodeData
  (const struct ndn_Data *data, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_DynamicUInt8Array *output,
   size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  error = ndn_encodeTlvData
    (data, signedPortionBeginOffset, signedPortionEndOffset, &encoder);
  *encodingLength = encoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeData
  (struct ndn_Data *data, const uint8_t *input, size_t inputLength,
   size_t *signedPortionBeginOffset, size_t *signedPortionEndOffset)
{
  struct ndn_TlvDecoder decoder;
  ndn_TlvDecoder_initialize(&decoder, input, inputLength);
  return ndn_decodeTlvData
    (data, signedPortionBeginOffset, signedPortionEndOffset, &decoder);
}

ndn_Error
ndn_Tlv0_2WireFormat_encodeControlParameters
  (const struct ndn_ControlParameters *controlParameters,
   struct ndn_DynamicUInt8Array *output, size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  error = ndn_encodeTlvControlParameters(controlParameters, &encoder);
  *encodingLength = encoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeControlParameters
  (struct ndn_ControlParameters *controlParameters, const uint8_t *input,
   size_t inputLength)
{
  struct ndn_TlvDecoder decoder;
  ndn_TlvDecoder_initialize(&decoder, input, inputLength);
  return ndn_decodeTlvControlParameters(controlParameters, &decoder);
}

ndn_Error
ndn_Tlv0_2WireFormat_encodeControlResponse
  (const struct ndn_ControlResponse *controlResponse,
   struct ndn_DynamicUInt8Array *output, size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  error = ndn_encodeTlvControlResponse(controlResponse, &encoder);
  *encodingLength = encoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeControlResponse
  (struct ndn_ControlResponse *controlResponse, const uint8_t *input,
   size_t inputLength)
{
  struct ndn_TlvDecoder decoder;
  ndn_TlvDecoder_initialize(&decoder, input, inputLength);
  return ndn_decodeTlvControlResponse(controlResponse, &decoder);
}

ndn_Error
ndn_Tlv0_2WireFormat_encodeSignatureInfo
  (const struct ndn_Signature *signature, struct ndn_DynamicUInt8Array *output,
   size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  error = ndn_encodeTlvSignatureInfo(signature, &encoder);
  *encodingLength = encoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_encodeSignatureValue
  (const struct ndn_Signature *signature, struct ndn_DynamicUInt8Array *output,
   size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  error = ndn_TlvEncoder_writeBlobTlv
    (&encoder, ndn_Tlv_SignatureValue, &signature->signature);
  *encodingLength = encoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeSignatureInfoAndValue
  (struct ndn_Signature *signature, const uint8_t *signatureInfo,
   size_t signatureInfoLength, const uint8_t *signatureValue,
   size_t signatureValueLength)
{
  ndn_Error error;
  struct ndn_TlvDecoder decoder;

  ndn_TlvDecoder_initialize(&decoder, signatureInfo, signatureInfoLength);
  if ((error = ndn_decodeTlvSignatureInfo(signature, &decoder)))
    return error;

  ndn_TlvDecoder_initialize(&decoder, signatureValue, signatureValueLength);
  return ndn_TlvDecoder_readBlobTlv
    (&decoder, ndn_Tlv_SignatureValue, &signature->signature);
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeLpPacket
  (struct ndn_LpPacket *lpPacket, const uint8_t *input, size_t inputLength)
{
  struct ndn_TlvDecoder decoder;
  ndn_TlvDecoder_initialize(&decoder, input, inputLength);
  return ndn_decodeTlvLpPacket(lpPacket, &decoder);
}

ndn_Error
ndn_Tlv0_2WireFormat_encodeDelegationSet_Delegation
  (const struct ndn_DelegationSet_Delegation *delegation,
   struct ndn_DynamicUInt8Array *output, size_t offset, size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  if ((error = ndn_TlvEncoder_seek(&encoder, offset)))
    return error;

  error = ndn_encodeTlvDelegationSet_Delegation(delegation, &encoder);
  *encodingLength = encoder.offset - offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeDelegationSet_Delegation
  (struct ndn_DelegationSet_Delegation *delegation, const uint8_t *input,
   size_t inputLength, size_t *encodingLength)
{
  struct ndn_TlvDecoder decoder;
  ndn_Error error;

  ndn_TlvDecoder_initialize(&decoder, input, inputLength);
  error = ndn_decodeTlvDelegationSet_Delegation(delegation, &decoder);
  *encodingLength = decoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_encodeEncryptedContent
  (const struct ndn_EncryptedContent *encryptedContent,
   struct ndn_DynamicUInt8Array *output, size_t *encodingLength)
{
  ndn_Error error;
  struct ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, output);
  error = ndn_encodeTlvEncryptedContent(encryptedContent, &encoder);
  *encodingLength = encoder.offset;

  return error;
}

ndn_Error
ndn_Tlv0_2WireFormat_decodeEncryptedContent
  (struct ndn_EncryptedContent *encryptedContent, const uint8_t *input,
   size_t inputLength)
{
  struct ndn_TlvDecoder decoder;
  ndn_TlvDecoder_initialize(&decoder, input, inputLength);
  return ndn_decodeTlvEncryptedContent(encryptedContent, &decoder);
}
