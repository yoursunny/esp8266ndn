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

#ifndef NDN_BLOB_H
#define NDN_BLOB_H

#include <ndn-cpp/c/util/blob-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the ndn_Blob struct with the given value.
 * @param self pointer to the ndn_Blob struct.
 * @param value The pre-allocated buffer for the value, or 0 for none.
 * @param length The number of bytes in value.
 */
static __inline void ndn_Blob_initialize(struct ndn_Blob *self, const uint8_t *value, size_t length)
{
  self->value = value;
  self->length = length;
}

/**
 * Set this ndn_Blob struct to have the values from the other blob.
 * @param self A pointer to this ndn_Blob struct.
 * @param other A pointer to the other ndn_Blob to get values from.
 */
static __inline void
ndn_Blob_setFromBlob(struct ndn_Blob *self, const struct ndn_Blob *other)
{
  *self = *other;
}

#ifdef __cplusplus
}
#endif

#endif
