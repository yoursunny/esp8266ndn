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

#ifndef NDN_NDN_REALLOC_H
#define NDN_NDN_REALLOC_H

#include "dynamic-uint8-array.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Wrap the C stdlib realloc to convert to/from void * to uint8_t *.
 * This can be used by ndn_DynamicUInt8Array_initialize.
 * @param self This is ignored.
 * @param array the allocated array buffer to realloc.
 * @param length the length for the new array buffer.
 * @return the new allocated array buffer.
 */
uint8_t *ndn_realloc(struct ndn_DynamicUInt8Array *self, uint8_t *array, size_t length);

#ifdef __cplusplus
}
#endif

#endif
