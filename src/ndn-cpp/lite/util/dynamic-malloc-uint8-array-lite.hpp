/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2017-2018 Regents of the University of California.
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

#ifndef NDN_DYNAMIC_MALLOC_UINT8_ARRAY_LITE_HPP
#define NDN_DYNAMIC_MALLOC_UINT8_ARRAY_LITE_HPP

#include "dynamic-uint8-array-lite.hpp"

namespace ndn {

/**
 * A DynamicMallocUInt8ArrayLite extends DynamicUInt8ArrayLite to use malloc and
 * realloc to resize the array. Note that the destructor frees the memory; if
 * you continue to need the array, you must copy getArray() before this object
 * is destructed. We also disable the copy constructor and assignment operator
 * because we don't do reference counting of the malloced array.
 * If your lightweight platform doesn't support malloc then don't link to this
 * file, and use DynamicUInt8ArrayLite directly with a fixed-size array.
 */
class DynamicMallocUInt8ArrayLite : public DynamicUInt8ArrayLite {
public:
  /**
   * Create a DynamicMallocUInt8ArrayLite with a buffer of the initial length.
   * @param initialLength The initial size of the malloced array. This may be 0.
   */
  DynamicMallocUInt8ArrayLite(size_t initialLength);

  /**
   * Free the malloced array.
   */
  ~DynamicMallocUInt8ArrayLite();

private:
  /**
   * Implement the static realloc function using ::realloc.
   * @param self A pointer to this object.
   * @param array Should be the front of the vector.
   * @param length The new length for the vector.
   * @return The front of the allocated vector.
   */
  static uint8_t*
  realloc(struct ::ndn_DynamicUInt8Array *self, uint8_t *array, size_t length);

  // Disable the copy constructor and assignment operator.
  DynamicMallocUInt8ArrayLite(const DynamicMallocUInt8ArrayLite& other);
  DynamicMallocUInt8ArrayLite& operator=(const DynamicMallocUInt8ArrayLite& other);

  uint8_t *mallocArray_;
};

}

#endif
