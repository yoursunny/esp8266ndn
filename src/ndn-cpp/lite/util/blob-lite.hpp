/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
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

#ifndef NDN_BLOB_LITE_HPP
#define NDN_BLOB_LITE_HPP

#include "../../c/util/blob-types.h"

namespace ndn {

/**
 * A BlobLite holds a pointer to an immutable pre-allocated buffer and its length
 * This is like a JavaScript string which is a pointer to an immutable string.
 * It is OK to pass a pointer to the string because the new owner can't change
 * the bytes of the string.  However, like a JavaScript string, it is possible
 * to change the pointer, and so this does allow the copy constructor and
 * assignment to change the pointer.  Also remember that the pointer can be null.
 */
class BlobLite : private ndn_Blob {
public:
  /**
   * Create a BlobLite where the buf and size are 0.
   */
  BlobLite();

  /**
   * Create a BlobLite with the given buffer.
   * @param buf The pre-allocated buffer for the value, or 0 for none.
   * @param size The number of bytes in buf.
   */
  BlobLite(const uint8_t* buf, size_t size);

  /**
   * Return buf given to the constructor.
   */
  const uint8_t*
  buf() const { return value; }

  /**
   * Return size given to the constructor.
   */
  size_t
  size() const { return length; }

  /**
   * Check if the array pointer is null.
   * @return true if the buffer pointer is null, otherwise false.
   */
  bool
  isNull() const { return !value; }

  /**
   * Check if the value of this BlobLite equals the other BlobLite, using ndn_
   * memcmp.
   * @param other The other BlobLite to check.
   * @return True if this isNull and other isNull or if the bytes of this
   * blob equals the bytes of the other.
   */
  bool
  equals(const BlobLite& other) const;

  /**
   * Downcast the reference to the ndn_Blob struct to a BlobLite.
   * @param blob A reference to the ndn_Blob struct.
   * @return The same reference as BlobLite.
   */
  static BlobLite&
  downCast(ndn_Blob& blob) { return *(BlobLite*)&blob; }

  static const BlobLite&
  downCast(const ndn_Blob& blob) { return *(BlobLite*)&blob; }
};

}

#endif
