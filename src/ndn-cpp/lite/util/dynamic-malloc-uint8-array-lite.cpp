/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2017 Regents of the University of California.
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

#include <stdlib.h>
#include "../../lite/util/dynamic-malloc-uint8-array-lite.hpp"

namespace ndn {

DynamicMallocUInt8ArrayLite::DynamicMallocUInt8ArrayLite(size_t initialLength)
: DynamicUInt8ArrayLite(0, 0, &DynamicMallocUInt8ArrayLite::realloc)
{
  if (initialLength <= 0)
    // Some implementations don't like malloc(0), so increase.
    initialLength = 1;

  mallocArray_ = (uint8_t*)::malloc(initialLength);
  setArrayAndLength(mallocArray_, initialLength);
}

DynamicMallocUInt8ArrayLite::~DynamicMallocUInt8ArrayLite()
{
  if (mallocArray_) {
    ::free(mallocArray_);
    mallocArray_ = 0;
  }
}

uint8_t*
DynamicMallocUInt8ArrayLite::realloc
  (struct ::ndn_DynamicUInt8Array *self, uint8_t *array, size_t length)
{
  DynamicMallocUInt8ArrayLite *thisObject = (DynamicMallocUInt8ArrayLite *)self;

  if (array != thisObject->mallocArray_)
    // We don't expect this to ever happen. The caller didn't pass the array
    // from this object.
    return 0;

  thisObject->mallocArray_ = (uint8_t*)::realloc(array, length);
  return thisObject->mallocArray_;
}

}
