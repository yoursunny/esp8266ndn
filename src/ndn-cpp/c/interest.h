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

#ifndef NDN_INTEREST_H
#define NDN_INTEREST_H

#include <ndn-cpp/c/interest-types.h>
#include "name.h"
#include "key-locator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @param self pointer to the ndn_NameComponent struct
 * @param type one of the ndn_ExcludeType enum
 * @param component the pre-allocated buffer for the component value, only used if type is ndn_Exclude_COMPONENT
 * @param componentLength the number of bytes in value, only used if type is ndn_Exclude_COMPONENT
 */
static __inline void ndn_ExcludeEntry_initialize(struct ndn_ExcludeEntry *self, ndn_ExcludeType type, const uint8_t *component, size_t componentLength)
{
  self->type = type;
  ndn_NameComponent_initialize(&self->component, component, componentLength);
}

/**
 * Set this exclude entry to have the values from the other exclude entry.
 * @param self A pointer to this ndn_NameComponent struct.
 * @param other A pointer to the other ndn_NameComponent struct to get values from.
 */
static __inline void
ndn_ExcludeEntry_setFromExcludeEntry
  (struct ndn_ExcludeEntry *self, const struct ndn_ExcludeEntry *other)
{
  *self = *other;
}

/**
 * Initialize an ndn_Exclude struct with the entries array.
 * @param self A pointer to the ndn_Exclude struct.
 * @param entries the pre-allocated array of ndn_ExcludeEntry
 * @param maxEntries the number of elements in the allocated entries array
 */
static __inline void ndn_Exclude_initialize(struct ndn_Exclude *self, struct ndn_ExcludeEntry *entries, size_t maxEntries)
{
  self->entries = entries;
  self->maxEntries = maxEntries;
  self->nEntries = 0;
}

/**
 * Clear all the entries.
 */
static __inline void
ndn_Exclude_clear(struct ndn_Exclude *self)
{
  self->nEntries = 0;
}

/**
 * Append a new entry of type ndn_Exclude_ANY.
 * @param self A pointer to the ndn_Exclude struct.
 * @return 0 for success, or an error code if there is no more room in the
 * entries array (nEntries is already maxEntries).
 */
ndn_Error
ndn_Exclude_appendAny(struct ndn_Exclude *self);

/**
 * Append a new entry of type ndn_Exclude_COMPONENT with the given component
 * value.
 * @param self A pointer to the ndn_Exclude struct.
 * @param component The bytes of the component.  This does not copy the bytes.
 * @param componentLength The number of bytes in component.
 * @return 0 for success, or an error code if there is no more room in the
 * entries array (nEntries is already maxEntries).
 */
ndn_Error
ndn_Exclude_appendComponent
  (struct ndn_Exclude *self, const uint8_t* component, size_t componentLength);

/**
 * Set this exclude to have the values from the other exclude.
 * @param self A pointer to the ndn_Exclude struct.
 * @param other A pointer to the other ndn_Exclude struct to get values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's entries array.
 */
ndn_Error
ndn_Exclude_setFromExclude
  (struct ndn_Exclude *self, const struct ndn_Exclude *other);

/**
 * Initialize an ndn_Interest struct with the pre-allocated nameComponents and excludeEntries,
 * and defaults for all the values.
 * @param self pointer to the ndn_Interest struct
 * @param nameComponents the pre-allocated array of ndn_NameComponent
 * @param maxNameComponents the number of elements in the allocated nameComponents array
 * @param excludeEntries the pre-allocated array of ndn_ExcludeEntry
 * @param maxExcludeEntries the number of elements in the allocated excludeEntries array
 * @param keyNameComponents The pre-allocated array of ndn_NameComponent for the keyLocator.
 * @param maxKeyNameComponents The number of elements in the allocated keyNameComponents array.
 */
static __inline void ndn_Interest_initialize
  (struct ndn_Interest *self, struct ndn_NameComponent *nameComponents, size_t maxNameComponents,
   struct ndn_ExcludeEntry *excludeEntries, size_t maxExcludeEntries, struct ndn_NameComponent *keyNameComponents,
   size_t maxKeyNameComponents)
{
  ndn_Name_initialize(&self->name, nameComponents, maxNameComponents);
  self->minSuffixComponents = -1;
  self->maxSuffixComponents = -1;
  ndn_Exclude_initialize(&self->exclude, excludeEntries, maxExcludeEntries);
  self->childSelector = -1;
  self->mustBeFresh = 1;
  self->interestLifetimeMilliseconds = -1.0;
  ndn_Blob_initialize(&self->nonce, 0, 0);
  ndn_KeyLocator_initialize(&self->keyLocator, keyNameComponents, maxKeyNameComponents);
  ndn_Blob_initialize(&self->linkWireEncoding, 0, 0);
  self->selectedDelegationIndex = -1;
}

/**
 * Get the MustBeFresh flag.
 * @param self A pointer to the ndn_Interest struct.
 * @return 1 if must be fresh, otherwise 0.
 */
static __inline int ndn_Interest_getMustBeFresh(const struct ndn_Interest *self)
{
  return self->mustBeFresh;
}

/**
 * Set the MustBeFresh flag.
 * @param self A pointer to the ndn_Interest struct.
 * @param mustBeFresh 1 if the content must be fresh, otherwise 0. If
 * you do not set this flag, the default value is 1.
 * @return This Interest so that you can chain calls to update values.
 */
static __inline void
ndn_Interest_setMustBeFresh(struct ndn_Interest *self, int mustBeFresh)
{
  self->mustBeFresh = (mustBeFresh ? 1 : 0);
}

/**
 * Set this ndn_Interest struct to have the values from the other interest.
 * @param self A pointer to the ndn_Interest struct.
 * @param other A pointer to the other ndn_Interest to get values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's name components array or exclude entries array.
 */
static __inline ndn_Error
ndn_Interest_setFromInterest
  (struct ndn_Interest *self, const struct ndn_Interest *other)
{
  ndn_Error error;

  if (other == self)
    // Setting to itself. Do nothing.
    return NDN_ERROR_success;

  if ((error = ndn_Name_setFromName(&self->name, &other->name)))
    return error;
  self->minSuffixComponents = other->minSuffixComponents;
  self->maxSuffixComponents = other->maxSuffixComponents;
  if ((error = ndn_Exclude_setFromExclude(&self->exclude, &other->exclude)))
    return error;
  self->childSelector = other->childSelector;
  self->mustBeFresh = other->mustBeFresh;
  self->interestLifetimeMilliseconds = other->interestLifetimeMilliseconds;
  ndn_Blob_setFromBlob(&self->nonce, &other->nonce);
  if ((error = ndn_KeyLocator_setFromKeyLocator
       (&self->keyLocator, &other->keyLocator)))
    return error;
  ndn_Blob_setFromBlob(&self->linkWireEncoding, &other->linkWireEncoding);
  self->selectedDelegationIndex = other->selectedDelegationIndex;

  return NDN_ERROR_success;
}

#ifdef __cplusplus
}
#endif

#endif

