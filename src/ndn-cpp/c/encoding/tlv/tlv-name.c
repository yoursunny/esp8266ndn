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

ndn_Error
ndn_encodeTlvNameComponent
  (const struct ndn_NameComponent *component, struct ndn_TlvEncoder *encoder)
{
  unsigned int type = ndn_NameComponent_isImplicitSha256Digest(component) ?
    ndn_Tlv_ImplicitSha256DigestComponent : ndn_Tlv_NameComponent;
  return ndn_TlvEncoder_writeBlobTlv(encoder, type, &component->value);
}

ndn_Error
ndn_decodeTlvNameComponent
  (struct ndn_NameComponent *component, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  uint64_t type;
  size_t saveOffset;

  saveOffset = decoder->offset;
  if ((error = ndn_TlvDecoder_readVarNumber(decoder, &type)))
    return error;
  // Restore the position.
  ndn_TlvDecoder_seek(decoder, saveOffset);

  if ((error = ndn_TlvDecoder_readBlobTlv(decoder, type, &component->value)))
    return error;
  if (type == ndn_Tlv_ImplicitSha256DigestComponent)
    component->type = ndn_NameComponentType_IMPLICIT_SHA256_DIGEST;
  else
    component->type = ndn_NameComponentType_GENERIC;

  return NDN_ERROR_success;
}

ndn_Error
ndn_encodeTlvName
  (const struct ndn_Name *name, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_TlvEncoder *encoder)
{
  size_t nameValueLength = 0;
  size_t i;
  ndn_Error error;

  for (i = 0; i < name->nComponents; ++i)
    nameValueLength += ndn_TlvEncoder_sizeOfBlobTlv
      (name->components[i].type, &name->components[i].value);

  if ((error = ndn_TlvEncoder_writeTypeAndLength(encoder, ndn_Tlv_Name, nameValueLength)))
    return error;

  *signedPortionBeginOffset = encoder->offset;

  if (name->nComponents == 0)
    // There is no "final component", so set signedPortionEndOffset arbitrarily.
    *signedPortionEndOffset = *signedPortionBeginOffset;
  else {
    for (i = 0; i < name->nComponents; ++i) {
      if (i == name->nComponents - 1)
        // We will begin the final component.
        *signedPortionEndOffset = encoder->offset;

      if ((error = ndn_encodeTlvNameComponent(&name->components[i], encoder)))
        return error;
    }
  }

  return NDN_ERROR_success;
}

ndn_Error
ndn_decodeTlvName
  (struct ndn_Name *name, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, ndn_Tlv_Name, &endOffset)))
    return error;

  *signedPortionBeginOffset = decoder->offset;
  // In case there are no components, set signedPortionEndOffset arbitrarily.
  *signedPortionEndOffset = *signedPortionBeginOffset;

  name->nComponents = 0;
  while (decoder->offset < endOffset) {
    *signedPortionEndOffset = decoder->offset;

    // First append an empty component, then decode into it.
    if ((error = ndn_Name_appendComponent(name, 0, 0)))
      return error;
    if ((error = ndn_decodeTlvNameComponent
         (&name->components[name->nComponents - 1], decoder)))
      return error;
  }

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
