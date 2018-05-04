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

#ifndef NDN_DYNAMIC_UINT8_ARRAY_LITE_HPP
#define NDN_DYNAMIC_UINT8_ARRAY_LITE_HPP

#include "../../c/errors.h"
#include "../../c/util/dynamic-uint8-array-types.h"

namespace ndn {

/**
 * A DynamicUInt8ArrayLite holds a pointer to an allocated array, the length of
 * the allocated array, and an optional realloc function which can changes its
 * size.
 */
class DynamicUInt8ArrayLite : private ndn_DynamicUInt8Array {
public:
  /**
   * Create a DynamicUInt8ArrayLite with the given array buffer.
   * @param array The allocated array buffer. It is the caller's responsibility
   * to free this buffer if necessary when finished with it.
   * @param length The length of the allocated array buffer.
   * @param reallocFunction See ensureLength. This may be 0.
   */
  DynamicUInt8ArrayLite
    (uint8_t *array, size_t length, ndn_ReallocFunction reallocFunction);

  /**
   * Get the current allocated array.
   * @return The array.
   */
  uint8_t*
  getArray() { return array; }

  const uint8_t*
  getArray() const { return array; }

  /**
   * Get the current length of the allocated array.
   * @return The array length.
   */
  size_t
  getLength() const { return length; }

  /**
   * Ensure that getLength() is greater than or equal to length. If it is, just
   * return 0 for success. Otherwise, if the reallocFunction given to the
   * constructor is 0, then return an error. If not 0, call reallocFunction to
   * reallocate this object's array, and update the getLength() (which may be
   * greater than the given length).
   * @param length The needed minimum size for getLength().
   * @return 0 for success, else an error code if need to reallocate the array
   * but can't.
   */
  ndn_Error
  ensureLength(size_t length);

  /**
   * Copy value into this object's array at offset, using ensureLength to make
   * sure the array has enough length.
   * @param value The buffer to copy from.
   * @param valueLength The length of the value buffer.
   * @param offset The offset in this object's array to copy to.
   * @return 0 for success, else an error code if need to reallocate the array
   * but can't.
   */
  ndn_Error
  copy(const uint8_t *value, size_t valueLength, size_t offset);

  /**
   * Ensure that getLength() is greater than or equal to the given length. If it
   * is, just return 0 for success. Otherwise, if the the reallocFunction given
   * to the constructor is 0, then return an error.  If not 0, call
   * reallocFunction to reallocate this object's array, and update getLength()
   * (which may be greater than the given length).
   * @param length the needed minimum size for self->length
   * @return 0 for success, else an error code if need to reallocate the array
   * but can't.
   */
  ndn_Error
  ensureLengthFromBack(size_t length);

  /**
   * First call ensureLengthFromBack to make sure this object's array has
   * offsetFromBack bytes. Then copy value into this object's array starting
   * offsetFromBack bytes from the back of the array.
   * @param value The buffer to copy from.
   * @param valueLength The length of the value buffer.
   * @param offsetFromBack The offset from the back of the array to start
   * copying.
   * @return 0 for success, else an error code if need to reallocate the array
   * but can't.
   */
  ndn_Error
  copyFromBack(const uint8_t *value, size_t valueLength, size_t offsetFromBack);

  /**
   * Downcast the reference to the ndn_DynamicUInt8Array struct to a
   * DynamicUInt8ArrayLite.
   * @param dynamicArray A reference to the ndn_DynamicUInt8Array struct.
   * @return The same reference as DynamicUInt8ArrayLite.
   */
  static DynamicUInt8ArrayLite&
  downCast(ndn_DynamicUInt8Array& dynamicArray)
  {
    return *(DynamicUInt8ArrayLite*)&dynamicArray;
  }

  static const DynamicUInt8ArrayLite&
  downCast(const ndn_DynamicUInt8Array& dynamicArray)
  {
    return *(DynamicUInt8ArrayLite*)&dynamicArray;
  }

protected:
  /**
   * Set the array and length. This is typically called by a derived class's
   * constructor after setting up storage.
   * @param array The allocated array buffer.
   * @param length The length of the allocated array buffer.
   */
  void
  setArrayAndLength(uint8_t *array, size_t length)
  {
    this->array = array;
    this->length = length;
  }

private:
  // Declare friends who can downcast to the private base.
  friend class TcpTransportLite;
  friend class UdpTransportLite;
  friend class UnixTransportLite;
  friend class ArduinoYunTcpTransportLite;
  friend class Esp8266TcpTransportLite;
  friend class Tlv0_2WireFormatLite;
};

}

#endif
