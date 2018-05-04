/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
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

#include "../../c/encoding/tlv-0_2-wire-format.h"
#include "../../lite/encoding/tlv-0_2-wire-format-lite.hpp"

namespace ndn {

ndn_Error
Tlv0_2WireFormatLite::encodeName
  (const NameLite& name, size_t* signedPortionBeginOffset,
   size_t* signedPortionEndOffset, DynamicUInt8ArrayLite& output,
   size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeName
    (&name, signedPortionBeginOffset, signedPortionEndOffset,
     &output, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeName
  (NameLite& name, const uint8_t* input, size_t inputLength,
   size_t* signedPortionBeginOffset, size_t* signedPortionEndOffset)
{
  return ndn_Tlv0_2WireFormat_decodeName
    (&name, input, inputLength, signedPortionBeginOffset,
     signedPortionEndOffset);
}

ndn_Error
Tlv0_2WireFormatLite::encodeInterest
  (const InterestLite& interest, size_t* signedPortionBeginOffset,
   size_t* signedPortionEndOffset, DynamicUInt8ArrayLite& output,
   size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeInterest
    (&interest, signedPortionBeginOffset, signedPortionEndOffset,
     &output, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeInterest
  (InterestLite& interest, const uint8_t* input, size_t inputLength,
   size_t* signedPortionBeginOffset, size_t* signedPortionEndOffset)
{
  return ndn_Tlv0_2WireFormat_decodeInterest
    (&interest, input, inputLength, signedPortionBeginOffset,
     signedPortionEndOffset);
}

ndn_Error
Tlv0_2WireFormatLite::encodeData
  (const DataLite& data, size_t* signedPortionBeginOffset,
   size_t* signedPortionEndOffset, DynamicUInt8ArrayLite& output,
   size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeData
    (&data, signedPortionBeginOffset, signedPortionEndOffset, &output,
     encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeData
  (DataLite& data, const uint8_t* input, size_t inputLength,
   size_t* signedPortionBeginOffset, size_t* signedPortionEndOffset)
{
  return ndn_Tlv0_2WireFormat_decodeData
    (&data, input, inputLength, signedPortionBeginOffset,
     signedPortionEndOffset);
}

ndn_Error
Tlv0_2WireFormatLite::encodeControlParameters
  (const ControlParametersLite& controlParameters,
   DynamicUInt8ArrayLite& output, size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeControlParameters
    (&controlParameters, &output, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeControlParameters
  (ControlParametersLite& controlParameters, const uint8_t* input,
   size_t inputLength)
{
  return ndn_Tlv0_2WireFormat_decodeControlParameters
    (&controlParameters, input, inputLength);
}

ndn_Error
Tlv0_2WireFormatLite::encodeControlResponse
  (const ControlResponseLite& controlResponse,
   DynamicUInt8ArrayLite& output, size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeControlResponse
    (&controlResponse, &output, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeControlResponse
  (ControlResponseLite& controlResponse, const uint8_t* input,
   size_t inputLength)
{
  return ndn_Tlv0_2WireFormat_decodeControlResponse
    (&controlResponse, input, inputLength);
}

ndn_Error
Tlv0_2WireFormatLite::encodeSignatureInfo
  (const SignatureLite& signature, DynamicUInt8ArrayLite& output,
   size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeSignatureInfo
    (&signature, &output, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::encodeSignatureValue
  (const SignatureLite& signature, DynamicUInt8ArrayLite& output,
   size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeSignatureValue
    (&signature, &output, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeSignatureInfoAndValue
  (SignatureLite& signature, const uint8_t *signatureInfo,
   size_t signatureInfoLength, const uint8_t *signatureValue,
   size_t signatureValueLength)
{
  return ndn_Tlv0_2WireFormat_decodeSignatureInfoAndValue
    (&signature, signatureInfo, signatureInfoLength, signatureValue,
     signatureValueLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeLpPacket
  (LpPacketLite& lpPacket, const uint8_t* input, size_t inputLength)
{
  return ndn_Tlv0_2WireFormat_decodeLpPacket(&lpPacket, input, inputLength);
}

ndn_Error
Tlv0_2WireFormatLite::encodeDelegationSet_Delegation
  (const DelegationSetLite::Delegation& delegation,
   DynamicUInt8ArrayLite& output, size_t offset, size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeDelegationSet_Delegation
    (&delegation, &output, offset, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeDelegationSet_Delegation
  (DelegationSetLite::Delegation& delegation, const uint8_t* input,
   size_t inputLength, size_t *encodingLength)
{
  return ndn_Tlv0_2WireFormat_decodeDelegationSet_Delegation
    (&delegation, input, inputLength, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::encodeEncryptedContent
  (const EncryptedContentLite& encryptedContent,
   DynamicUInt8ArrayLite& output, size_t* encodingLength)
{
  return ndn_Tlv0_2WireFormat_encodeEncryptedContent
    (&encryptedContent, &output, encodingLength);
}

ndn_Error
Tlv0_2WireFormatLite::decodeEncryptedContent
  (EncryptedContentLite& encryptedContent, const uint8_t* input,
   size_t inputLength)
{
  return ndn_Tlv0_2WireFormat_decodeEncryptedContent
    (&encryptedContent, input, inputLength);
}

}
