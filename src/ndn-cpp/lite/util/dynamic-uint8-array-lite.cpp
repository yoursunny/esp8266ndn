/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2015-2016 Regents of the University of California.
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

#include "../../c/util/dynamic-uint8-array.h"
#include <ndn-cpp/lite/util/dynamic-uint8-array-lite.hpp>

namespace ndn {

DynamicUInt8ArrayLite::DynamicUInt8ArrayLite
  (uint8_t *array, size_t length, ndn_ReallocFunction reallocFunction)
{
  ndn_DynamicUInt8Array_initialize(this, array, length, reallocFunction);
}

ndn_Error
DynamicUInt8ArrayLite::ensureLength(size_t length)
{
  return ndn_DynamicUInt8Array_ensureLength(this, length);
}

ndn_Error
DynamicUInt8ArrayLite::copy
  (const uint8_t *value, size_t valueLength, size_t offset)
{
  return ndn_DynamicUInt8Array_copy(this, value, valueLength, offset);
}

ndn_Error
DynamicUInt8ArrayLite::ensureLengthFromBack(size_t length)
{
  return ndn_DynamicUInt8Array_ensureLengthFromBack(this, length);
}

ndn_Error
DynamicUInt8ArrayLite::copyFromBack
  (const uint8_t *value, size_t valueLength, size_t offsetFromBack)
{
  return ndn_DynamicUInt8Array_copyFromBack
    (this, value, valueLength, offsetFromBack);
}

}
