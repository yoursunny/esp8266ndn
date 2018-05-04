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

#include "dynamic-uint8-array.h"

ndn_Error ndn_DynamicUInt8Array_reallocArray(struct ndn_DynamicUInt8Array *self, size_t length)
{
  size_t newLength;
  uint8_t *newArray;

  if (!self->realloc)
    return NDN_ERROR_DynamicUInt8Array_realloc_function_pointer_not_supplied;

  // See if double is enough.
  newLength = self->length * 2;
  if (length > newLength)
    // The needed length is much greater, so use it.
    newLength = length;

  newArray = (*self->realloc)(self, self->array, newLength);
  if (!newArray)
    return NDN_ERROR_DynamicUInt8Array_realloc_failed;

  self->array = newArray;
  self->length = newLength;

  return NDN_ERROR_success;
}

ndn_Error ndn_DynamicUInt8Array_reallocArrayFromBack
  (struct ndn_DynamicUInt8Array *self, size_t length)
{
  size_t initialLength = self->length;

  ndn_Error error;
  if ((error = ndn_DynamicUInt8Array_reallocArray(self, length)))
    return error;

  // Shift to the back.
  // TODO: We should use ndn_memmove, but we need to add support in ndn_memory.h.
  if (initialLength > 0) {
    uint8_t *to = self->array + self->length;
    uint8_t *from = self->array + initialLength;
    do {
      *(--to) = *(--from);
    } while (from != self->array);
  }

  return NDN_ERROR_success;
}
