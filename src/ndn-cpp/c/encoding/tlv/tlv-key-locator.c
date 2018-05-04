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

ndn_Error
ndn_encodeTlvKeyLocatorValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_KeyLocator *keyLocator = (struct ndn_KeyLocator *)context;
  ndn_Error error;

  if ((int)keyLocator->type < 0)
    return NDN_ERROR_success;

  if (keyLocator->type == ndn_KeyLocatorType_KEYNAME) {
    size_t dummyBeginOffset, dummyEndOffset;
    if ((error = ndn_encodeTlvName
         (&keyLocator->keyName, &dummyBeginOffset, &dummyEndOffset, encoder)))
      return error;
  }
  else if (keyLocator->type == ndn_KeyLocatorType_KEY_LOCATOR_DIGEST && keyLocator->keyData.length > 0) {
    if ((error = ndn_TlvEncoder_writeBlobTlv(encoder, ndn_Tlv_KeyLocatorDigest, &keyLocator->keyData)))
      return error;
  }
  else
    return NDN_ERROR_unrecognized_ndn_KeyLocatorType;

  return NDN_ERROR_success;
}

ndn_Error
ndn_decodeTlvKeyLocator
  (unsigned int expectedType, struct ndn_KeyLocator *keyLocator,
   struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  int gotExpectedType;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, expectedType, &endOffset)))
    return error;

  ndn_KeyLocator_initialize
    (keyLocator, keyLocator->keyName.components, keyLocator->keyName.maxComponents);

  if (decoder->offset == endOffset)
    // The KeyLocator is omitted, so leave the fields as none.
    return NDN_ERROR_success;

  if ((error = ndn_TlvDecoder_peekType(decoder, ndn_Tlv_Name, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    size_t dummyBeginOffset, dummyEndOffset;

    // KeyLocator is a Name.
    if ((error = ndn_decodeTlvName
         (&keyLocator->keyName, &dummyBeginOffset, &dummyEndOffset, decoder)))
      return error;
    keyLocator->type = ndn_KeyLocatorType_KEYNAME;
  }
  else {
    if ((error = ndn_TlvDecoder_peekType(decoder, ndn_Tlv_KeyLocatorDigest, endOffset, &gotExpectedType)))
      return error;
    if (gotExpectedType) {
      // KeyLocator is a KeyLocatorDigest.
      keyLocator->type = ndn_KeyLocatorType_KEY_LOCATOR_DIGEST;
      if ((error = ndn_TlvDecoder_readBlobTlv(decoder, ndn_Tlv_KeyLocatorDigest, &keyLocator->keyData)))
        return error;
    }
    else
      return NDN_ERROR_decodeKeyLocator_unrecognized_key_locator_type;
  }

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
