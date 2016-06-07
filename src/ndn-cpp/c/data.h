/**
 * Copyright (C) 2013-2016 Regents of the University of California.
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

#ifndef NDN_DATA_H
#define NDN_DATA_H

#include <math.h>
#include <ndn-cpp/c/data-types.h>
#include "name.h"
#include "key-locator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the ndn_Signature struct with values for none.
 * @param self A pointer to the ndn_Signature struct.
 * @param keyNameComponents The pre-allocated array of ndn_NameComponent for the keyLocator.
 * @param maxKeyNameComponents The number of elements in the allocated keyNameComponents array.
 */
static __inline void ndn_Signature_initialize(struct ndn_Signature *self, struct ndn_NameComponent *keyNameComponents, size_t maxKeyNameComponents) {
  self->type = (ndn_SignatureType)-1;
  ndn_Blob_initialize(&self->signature, 0, 0);
  ndn_Blob_initialize(&self->signatureInfoEncoding, 0, 0);
  self->genericTypeCode = -1;
  ndn_KeyLocator_initialize(&self->keyLocator, keyNameComponents, maxKeyNameComponents);
}

/**
 * Set the fields of the ndn_Signature struct to the values from
 * ndn_Signature_initialize.
 * @param self A pointer to the ndn_Signature struct.
 */
static __inline void ndn_Signature_clear(struct ndn_Signature *self)
{
  ndn_Signature_initialize
    (self, self->keyLocator.keyName.components,
     self->keyLocator.keyName.maxComponents);
}

/**
 * Set this ndn_Signature struct to have the values from the other signature.
 * @param self A pointer to the ndn_Signature struct.
 * @param other A pointer to the other ndn_Signature to get values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's key locator keyName components array.
 */
static __inline ndn_Error
ndn_Signature_setFromSignature
  (struct ndn_Signature *self, const struct ndn_Signature *other)
{
  ndn_Error error;

  if (other == self)
    // Setting to itself. Do nothing.
    return NDN_ERROR_success;

  self->type = other->type;
  ndn_Blob_setFromBlob(&self->signature, &other->signature);
  ndn_Blob_setFromBlob(&self->signatureInfoEncoding, &other->signatureInfoEncoding);
  self->genericTypeCode = other->genericTypeCode;
  if ((error = ndn_KeyLocator_setFromKeyLocator
       (&self->keyLocator, &other->keyLocator)))
    return error;

  return NDN_ERROR_success;
}

/**
 * Initialize the ndn_MetaInfo struct with values for none and the type to the default ndn_ContentType_BLOB.
 * @param self A pointer to the ndn_MetaInfo struct.
 */
static __inline void ndn_MetaInfo_initialize(struct ndn_MetaInfo *self)
{
  self->timestampMilliseconds = -1;
  self->type = ndn_ContentType_BLOB;
  self->freshnessPeriod = -1;
  ndn_NameComponent_initialize(&self->finalBlockId, 0, 0);
}

/**
 * @deprecated Use freshnessPeriod.
 */
static __inline int ndn_MetaInfo_getFreshnessSeconds(const struct ndn_MetaInfo *self)
{
  return self->freshnessPeriod < 0 ? -1 : (int)round(self->freshnessPeriod / 1000.0);
}

/**
 * @deprecated Use freshnessPeriod.
 */
static __inline void ndn_MetaInfo_setFreshnessSeconds(struct ndn_MetaInfo *self, int freshnessSeconds)
{
  self->freshnessPeriod = freshnessSeconds < 0 ? -1.0 : (double)freshnessSeconds * 1000.0;
}

/**
 * Set this ndn_MetaInfo struct to have the values from the other meta info.
 * @param self A pointer to the ndn_MetaInfo struct.
 * @param other A pointer to the other ndn_MetaInfo to get values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's array.
 */
static __inline ndn_Error
ndn_MetaInfo_setFromMetaInfo
  (struct ndn_MetaInfo *self, const struct ndn_MetaInfo *other)
{
  if (other == self)
    // Setting to itself. Do nothing.
    return NDN_ERROR_success;

  self->timestampMilliseconds = other->timestampMilliseconds;
  self->type = other->type;
  self->freshnessPeriod = other->freshnessPeriod;
  ndn_NameComponent_setFromNameComponent(&self->finalBlockId, &other->finalBlockId);

  return NDN_ERROR_success;
}

/**
 * Initialize an ndn_Data struct with the pre-allocated nameComponents and keyNameComponents,
 * and defaults for all the values.
 * @param self A pointer to the ndn_Data struct.
 * @param nameComponents The pre-allocated array of ndn_NameComponent.
 * @param maxNameComponents The number of elements in the allocated nameComponents array.
 * @param keyNameComponents The pre-allocated array of ndn_NameComponent for the signature.keyLocator.
 * @param maxKeyNameComponents The number of elements in the allocated keyNameComponents array.
 */
static __inline void ndn_Data_initialize
  (struct ndn_Data *self, struct ndn_NameComponent *nameComponents, size_t maxNameComponents,
   struct ndn_NameComponent *keyNameComponents, size_t maxKeyNameComponents)
{
  ndn_Signature_initialize(&self->signature, keyNameComponents, maxKeyNameComponents);
  ndn_Name_initialize(&self->name, nameComponents, maxNameComponents);
  ndn_MetaInfo_initialize(&self->metaInfo);
  ndn_Blob_initialize(&self->content, 0, 0);
}

/**
 * Set this ndn_Data struct to have the values from the other data.
 * @param self A pointer to the ndn_Data struct.
 * @param other A pointer to the other ndn_Data to get values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's name or key locator keyName components array.
 */
static __inline ndn_Error
ndn_Data_setFromData(struct ndn_Data *self, const struct ndn_Data *other)
{
  ndn_Error error;

  if (other == self)
    // Setting to itself. Do nothing.
    return NDN_ERROR_success;

  if ((error = ndn_Signature_setFromSignature
       (&self->signature, &other->signature)))
    return error;
  if ((error = ndn_Name_setFromName(&self->name, &other->name)))
    return error;
  if ((error = ndn_MetaInfo_setFromMetaInfo
       (&self->metaInfo, &other->metaInfo)))
    return error;
  ndn_Blob_setFromBlob(&self->content, &other->content);

  return NDN_ERROR_success;
}

#ifdef __cplusplus
}
#endif

#endif
