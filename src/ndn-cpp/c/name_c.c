/**
 * Copyright (C) 2013-2018 Regents of the University of California.
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

#include <string.h>
#include "util/ndn_memory.h"
#include "encoding/tlv/tlv-encoder.h"
#include "name.h"

uint64_t ndn_NameComponent_toNumber(const struct ndn_NameComponent *self)
{
  uint64_t result = 0;
  size_t i;
  for (i = 0; i < self->value.length; ++i) {
    result *= 256;
    result += (uint64_t)self->value.value[i];
  }

  return result;
}

ndn_Error ndn_NameComponent_toNumberWithMarker
  (const struct ndn_NameComponent *self, uint8_t marker, uint64_t *result)
{
  uint64_t localResult;
  size_t i;

  if (self->value.length == 0 || self->value.value[0] != marker)
    return NDN_ERROR_Name_component_does_not_begin_with_the_expected_marker;

  localResult = 0;
  for (i = 1; i < self->value.length; ++i) {
    localResult *= 256;
    localResult += (uint64_t)self->value.value[i];
  }

  *result = localResult;
  return NDN_ERROR_success;
}

ndn_Error ndn_NameComponent_toNumberWithPrefix
  (const struct ndn_NameComponent *self, const uint8_t *prefix,
   size_t prefixLength, uint64_t *result)
{
  uint64_t localResult;
  size_t i;

  if (self->value.length < prefixLength || ndn_memcmp(self->value.value, prefix, prefixLength) != 0)
    return NDN_ERROR_Name_component_does_not_begin_with_the_expected_marker;

  localResult = 0;
  for (i = prefixLength; i < self->value.length; ++i) {
    localResult *= 256;
    localResult += (uint64_t)self->value.value[i];
  }

  *result = localResult;
  return NDN_ERROR_success;
}

int
ndn_NameComponent_hasPrefix
  (const struct ndn_NameComponent *self, const uint8_t *prefix,
   size_t prefixLength)
{
  if (self->value.length >= prefixLength &&
      ndn_memcmp(self->value.value, prefix, prefixLength) == 0)
    return 1;
  else
    return 0;
}

ndn_Error
ndn_NameComponent_setFromNumber
  (struct ndn_NameComponent *self, uint64_t number, uint8_t *buffer,
   size_t bufferLength)
{
  struct ndn_DynamicUInt8Array output;
  struct ndn_TlvEncoder encoder;
  ndn_Error error;

  ndn_DynamicUInt8Array_initialize(&output, buffer, bufferLength, 0);
  ndn_TlvEncoder_initialize(&encoder, &output);

  if ((error = ndn_TlvEncoder_writeNonNegativeInteger(&encoder, number)))
    return error;
  // This sets the type to ndn_NameComponentType_GENERIC.
  ndn_NameComponent_initialize(self, buffer, encoder.offset);

  return NDN_ERROR_success;
}

ndn_Error
ndn_NameComponent_setFromNumberWithMarker
  (struct ndn_NameComponent *self, uint64_t number, uint8_t marker,
   uint8_t *buffer, size_t bufferLength)
{
  struct ndn_DynamicUInt8Array output;
  struct ndn_TlvEncoder encoder;
  ndn_Error error;

  ndn_DynamicUInt8Array_initialize(&output, buffer, bufferLength, 0);
  ndn_TlvEncoder_initialize(&encoder, &output);

  // Add the leading marker.
  if ((error = ndn_TlvEncoder_writeNonNegativeInteger(&encoder, marker & 0xff)))
    return error;
  if ((error = ndn_TlvEncoder_writeNonNegativeInteger(&encoder, number)))
    return error;
  // This sets the type to ndn_NameComponentType_GENERIC.
  ndn_NameComponent_initialize(self, buffer, encoder.offset);

  return NDN_ERROR_success;
}

ndn_Error
ndn_NameComponent_setImplicitSha256Digest
  (struct ndn_NameComponent *self, const uint8_t* digest, size_t digestLength)
{
  if (digestLength != ndn_SHA256_DIGEST_SIZE)
    return NDN_ERROR_Incorrect_digest_size;

  ndn_NameComponent_initialize(self, digest, digestLength);
  self->type = ndn_NameComponentType_IMPLICIT_SHA256_DIGEST;
  return NDN_ERROR_success;
}

int ndn_NameComponent_equals
  (const struct ndn_NameComponent *self, const struct ndn_NameComponent *other)
{
  return self->value.length == other->value.length &&
         ndn_memcmp(self->value.value, other->value.value,
                    self->value.length) == 0 &&
         self->type == other->type;
}

int ndn_NameComponent_compare
  (const struct ndn_NameComponent *self, const struct ndn_NameComponent *other)
{
  if (self->type < other->type)
    return -1;
  if (self->type > other->type)
    return 1;

  if (self->value.length < other->value.length)
    return -1;
  if (self->value.length > other->value.length)
    return 1;

  // The components are equal length.  Just do a byte compare.
  return ndn_memcmp(self->value.value, other->value.value, self->value.length);
}

int ndn_Name_equals(const struct ndn_Name *self, const struct ndn_Name *name)
{
  int i;

  if (self->nComponents != name->nComponents)
    return 0;

  // Check from last to first since the last components are more likely to differ.
  for (i = self->nComponents - 1; i >= 0; --i) {
    struct ndn_NameComponent *selfComponent = self->components + i;
    struct ndn_NameComponent *nameComponent = name->components + i;

    if (!ndn_NameComponent_equals(selfComponent, nameComponent))
      return 0;
  }

  return 1;
}

int ndn_Name_match(const struct ndn_Name *self, const struct ndn_Name *name)
{
  int i;

  // This name is longer than the name we are checking it against.
  if (self->nComponents > name->nComponents)
    return 0;

  // Check if at least one of given components doesn't match. Check from last to
  // first since the last components are more likely to differ.
  for (i = self->nComponents - 1; i >= 0; --i) {
    struct ndn_NameComponent *selfComponent = self->components + i;
    struct ndn_NameComponent *nameComponent = name->components + i;

    if (selfComponent->value.length != nameComponent->value.length ||
        ndn_memcmp(selfComponent->value.value, nameComponent->value.value, selfComponent->value.length) != 0)
      return 0;
  }

  return 1;
}

ndn_Error ndn_Name_appendComponent(struct ndn_Name *self, const uint8_t *value, size_t valueLength)
{
  if (self->nComponents >= self->maxComponents)
      return NDN_ERROR_attempt_to_add_a_component_past_the_maximum_number_of_components_allowed_in_the_name;
  ndn_NameComponent_initialize(self->components + self->nComponents, value, valueLength);
  ++self->nComponents;

  return NDN_ERROR_success;
}

ndn_Error ndn_Name_appendNameComponent
  (struct ndn_Name *self, const struct ndn_NameComponent *component)
{
  ndn_Error error;
  // Use ndn_Name_appendComponent which checks for max name components.
  if ((error = ndn_Name_appendComponent(self, 0, 0)))
    return error;

  // Copy the whole component including the type.
  ndn_NameComponent_setFromNameComponent
    (&self->components[self->nComponents - 1], component);
  return NDN_ERROR_success;
}

ndn_Error ndn_Name_appendName(struct ndn_Name *self, const struct ndn_Name *name)
{
  ndn_Error error;
  size_t i;

  for (i = 0; i < name->nComponents; ++i) {
    if ((error = ndn_Name_appendNameComponent(self, &name->components[i])))
      return error;
  }

  return NDN_ERROR_success;
}

ndn_Error ndn_Name_appendString(struct ndn_Name *self, const char *value)
{
  return ndn_Name_appendComponent(self, (const uint8_t *)value, strlen(value));
}

ndn_Error
ndn_Name_setFromName(struct ndn_Name *self, const struct ndn_Name *other)
{
  size_t i;
  if (other == self)
    // Setting to itself. Do nothing.
    return NDN_ERROR_success;

  if (other->nComponents > self->maxComponents)
    return NDN_ERROR_attempt_to_add_a_component_past_the_maximum_number_of_components_allowed_in_the_name;

  self->nComponents = other->nComponents;
  // If the two names share the components array, we don't need to copy.
  if (self->components != other->components) {
    for (i = 0; i < other->nComponents; ++i)
      ndn_NameComponent_setFromNameComponent
        (&self->components[i], &other->components[i]);
  }

  return NDN_ERROR_success;
}
