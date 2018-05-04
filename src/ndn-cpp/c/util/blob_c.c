/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#include "ndn_memory.h"
#include "blob.h"

int
ndn_Blob_equals(const struct ndn_Blob *self, const struct ndn_Blob *other)
{
  if (ndn_Blob_isNull(self))
    return ndn_Blob_isNull(other);
  else if (ndn_Blob_isNull(other))
    return 0;
  else {
    if (self->length != other->length)
      return 0;

    return ndn_memcmp(self->value, other->value, self->length) == 0;
  }
}
