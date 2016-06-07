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

#ifndef NDN_DYNAMIC_UINT8_ARRAY_H
#define NDN_DYNAMIC_UINT8_ARRAY_H

#include <ndn-cpp/c/errors.h>
#include "ndn_memory.h"
#include <ndn-cpp/c/util/dynamic-uint8-array-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_DynamicUInt8Array struct with the given array buffer.
 * @param self pointer to the ndn_DynamicUInt8Array struct
 * @param array The allocated array buffer. It is the caller's responsibility to
 * free this buffer if necessary when finished with it.
 * @param length the length of the allocated array buffer
 * @param reallocFunction see ndn_DynamicUInt8Array_ensureLength.  This may be 0.
 */
static __inline void ndn_DynamicUInt8Array_initialize
  (struct ndn_DynamicUInt8Array *self, uint8_t *array, size_t length,
   ndn_ReallocFunction reallocFunction)
{
  self->array = array;
  self->length = length;
  self->realloc = reallocFunction;
}

/**
 * Do the work of ndn_DynamicUInt8Array_ensureLength if realloc is necessary.
 * If the self->reallocFunction function pointer is null, then return an error.
 * If not null, call self->reallocFunction to reallocate self->array, and
 * update self->length (which may be greater than length).
 * @param self pointer to the ndn_DynamicUInt8Array struct
 * @param length the needed minimum size for self->length
 * @return 0 for success, else an error code if can't reallocate the array
 */
ndn_Error ndn_DynamicUInt8Array_reallocArray
  (struct ndn_DynamicUInt8Array *self, size_t length);

/**
 * Do the work of ndn_DynamicUInt8Array_ensureLengthFromBack if realloc is necessary.
 * If the self->reallocFunction function pointer is null, then return an error.
 * If not null, call self->reallocFunction to reallocate self->array, and
 * update self->length (which may be greater than length).  Then, shift the
 * bytes to the back of the array.  This may result in copying the bytes once
 * for the realloc and again to shift, but this is difficult to avoid given how
 * most realloc functions work.
 * @param self A pointer to the ndn_DynamicUInt8Array struct.
 * @param length The needed minimum size for self->length.
 * @return 0 for success, else an error code if can't reallocate the array.
 */
ndn_Error ndn_DynamicUInt8Array_reallocArrayFromBack
  (struct ndn_DynamicUInt8Array *self, size_t length);

/**
 * Ensure that self->length is greater than or equal to length.  If it is, just
 * return 0 for success. Otherwise, if the self->reallocFunction function
 * pointer is null, then return an error. If not null, call
 * self->reallocFunction to reallocate self->array, and update self->length
 * (which may be greater than length).
 * @param self pointer to the ndn_DynamicUInt8Array struct
 * @param length the needed minimum size for self->length
 * @return 0 for success, else an error code if need to reallocate the array but can't
 */
static __inline ndn_Error ndn_DynamicUInt8Array_ensureLength
  (struct ndn_DynamicUInt8Array *self, size_t length)
{
  if (self->length >= length)
    return NDN_ERROR_success;

  return ndn_DynamicUInt8Array_reallocArray(self, length);
}

/**
 * Copy value into self->array at offset, using
 * ndn_DynamicUInt8Array_ensureLength to make sure self->array has enough length.
 * @param self pointer to the ndn_DynamicUInt8Array struct
 * @param value the buffer to copy from
 * @param valueLength the length of the value buffer
 * @param offset the offset in self->array to copy to
 * @return 0 for success, else an error code if need to reallocate the array but can't
 */
static __inline ndn_Error ndn_DynamicUInt8Array_copy
  (struct ndn_DynamicUInt8Array *self, const uint8_t *value, size_t valueLength,
   size_t offset)
{
  ndn_Error error;
  if ((error = ndn_DynamicUInt8Array_ensureLength(self, valueLength + offset)))
    return error;
  ndn_memcpy(self->array + offset, value, valueLength);
  return NDN_ERROR_success;
};

/**
 * Ensure that self->length is greater than or equal to length.  If it is, just
 * return 0 for success. Otherwise, if the self->realloc function pointer is
 * null, then return an error.  If not null, call self->realloc to reallocate
 * self->array, and update self->length (which may be greater than length).
 * @param self pointer to the ndn_DynamicUInt8Array struct
 * @param length the needed minimum size for self->length
 * @return 0 for success, else an error code if need to reallocate the array but
 * can't.
 */
static __inline ndn_Error ndn_DynamicUInt8Array_ensureLengthFromBack
  (struct ndn_DynamicUInt8Array *self, size_t length)
{
  if (self->length >= length)
    return NDN_ERROR_success;

  return ndn_DynamicUInt8Array_reallocArrayFromBack(self, length);
}

/**
 * First call ndn_DynamicUInt8Array_ensureLengthFromBack to make sure
 * self->array has offsetFromBack bytes. Then copy value into self->array
 * starting offsetFromBack bytes from the back of the array.
 * @param self A pointer to the ndn_DynamicUInt8Array struct.
 * @param value The buffer to copy from.
 * @param valueLength The length of the value buffer.
 * @param offsetFromBack The offset from the back of the array to start
 * copying.
 * @return 0 for success, else an error code if need to reallocate the array but
 * can't.
 */
static __inline ndn_Error ndn_DynamicUInt8Array_copyFromBack
  (struct ndn_DynamicUInt8Array *self, const uint8_t *value, size_t valueLength,
   size_t offsetFromBack)
{
  ndn_Error error;
  if ((error = ndn_DynamicUInt8Array_ensureLengthFromBack(self, offsetFromBack)))
    return error;
  ndn_memcpy(self->array + (self->length - offsetFromBack), value, valueLength);
  return NDN_ERROR_success;
};

#ifdef __cplusplus
}
#endif

#endif

