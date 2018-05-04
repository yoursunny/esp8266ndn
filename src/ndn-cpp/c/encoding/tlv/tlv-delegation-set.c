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

#include "tlv-name.h"
#include "tlv-delegation-set.h"

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs
 * in the body of the Delegation value.
 * @param context This is the ndn_DelegationSet_Delegation struct pointer which
 * was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeDelegationValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_DelegationSet_Delegation *delegation =
    (struct ndn_DelegationSet_Delegation *)context;
  ndn_Error error;
  size_t dummyBeginOffset, dummyEndOffset;

  if ((error = ndn_TlvEncoder_writeNonNegativeIntegerTlv
      (encoder, ndn_Tlv_Link_Preference, delegation->preference)))
    return error;
  if ((error = ndn_encodeTlvName
       (&delegation->name, &dummyBeginOffset, &dummyEndOffset, encoder)))
    return error;

  return NDN_ERROR_success;
}

ndn_Error
ndn_encodeTlvDelegationSet_Delegation
  (const struct ndn_DelegationSet_Delegation *delegation,
   struct ndn_TlvEncoder *encoder)
{
  return ndn_TlvEncoder_writeNestedTlv
    (encoder, ndn_Tlv_Link_Delegation, encodeDelegationValue, delegation, 0);
}

ndn_Error
ndn_decodeTlvDelegationSet_Delegation
  (struct ndn_DelegationSet_Delegation *delegation,
   struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  size_t dummyBeginOffset, dummyEndOffset;
  uint64_t unsignedPreference;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart
       (decoder, ndn_Tlv_Link_Delegation, &endOffset)))
    return error;

  if ((error = ndn_TlvDecoder_readNonNegativeIntegerTlv
       (decoder, ndn_Tlv_Link_Preference, &unsignedPreference)))
    return error;
  delegation->preference = (int)unsignedPreference;

  if ((error = ndn_decodeTlvName
       (&delegation->name, &dummyBeginOffset, &dummyEndOffset, decoder)))
    return error;

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}

