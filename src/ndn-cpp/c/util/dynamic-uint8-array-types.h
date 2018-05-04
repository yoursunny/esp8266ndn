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

#ifndef NDN_DYNAMIC_UINT8_ARRAY_TYPES_H
#define NDN_DYNAMIC_UINT8_ARRAY_TYPES_H

#include "../common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ndn_ReallocFunction is a function pointer used by ndn_DynamicUInt8Array.
 */
struct ndn_DynamicUInt8Array;
typedef uint8_t * (*ndn_ReallocFunction)
  (struct ndn_DynamicUInt8Array *self, uint8_t *array, size_t length);

/**
 * A struct ndn_DynamicUInt8Array holds a pointer to an allocated array, the
 * length of the allocated array, and an optional realloc function which can
 * changes its size.
 */
struct ndn_DynamicUInt8Array {
  uint8_t *array; /**< the allocated array buffer */
  size_t length;  /**< the length of the allocated array buffer */
  ndn_ReallocFunction realloc; /**< a pointer to a function that reallocates array and returns a new pointer to a buffer of
                                * length bytes, or 0 for error.  On success, the contents of the old buffer are copied to the new one.
                                * On success, the original array pointer will no longer be used.
                                * self is a pointer to the struct ndn_DynamicUInt8Array which is calling realloc.
                                * This function pointer may be 0 (which causes an error if a reallocate is necessary). */
};

#ifdef __cplusplus
}
#endif

#endif
