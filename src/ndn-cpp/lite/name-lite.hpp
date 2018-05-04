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

#ifndef NDN_NAME_LITE_HPP
#define NDN_NAME_LITE_HPP

#include "../c/errors.h"
#include "util/blob-lite.hpp"
#include "../c/name-types.h"

namespace ndn {

class ExcludeLite;

/**
 * A NameLite holds an array of NameLite::Component.
 */
class NameLite : public ndn_Name {
public:
  /**
   * A NameLite::Component holds a pointer to the component value.
   */
  class Component : private ndn_NameComponent {
  public:
    /**
     * Create a GENERIC NameLite::Component of zero size.
     */
    Component();

    /**
     * Create a GENERIC NameLite::Component with the given value.
     * @param value The pre-allocated buffer for the value.
     * @param valueLength The number of bytes in value.
     */
    Component(const uint8_t* value, size_t valueLength);

    /**
     * Create a GENERIC NameLite::Component taking the pointer and size from the
     * BlobLite value.
     * @param value The BlobLite with the pointer to use for this component.
     */
    Component(const BlobLite& value);

    const BlobLite&
    getValue() const { return BlobLite::downCast(value); }

    /**
     * Check if this component is a segment number according to NDN naming
     * conventions for "Segment number" (marker 0x00) and return an integer.
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @return True if this is a segment number.
     */
    bool
    isSegment() const;

    /**
     * Check if this component is a segment byte offset according to NDN naming
     * conventions for "Byte offset" (marker 0xFB) and return an integer.
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @return True if this is a segment byte offset.
     */
    bool
    isSegmentOffset() const;

    /**
     * Check if this component is a version number according to NDN naming
     * conventions for "Versioning" (marker 0xFD) and return an integer.
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @return True if this is a version number.
     */
    bool
    isVersion() const;

    /**
     * Check if this component is a timestamp according to NDN naming
     * conventions for "Timestamp" (marker 0xFC) and return an integer.
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @return True if this is a timestamp.
     */
    bool
    isTimestamp() const;

    /**
     * Check if this component is a sequence number according to NDN naming
     * conventions for "Sequencing" (marker 0xFE) and return an integer.
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @return True if this is a sequence number.
     */
    bool
    isSequenceNumber() const;

    /**
     * Check if this component is a generic component.
     * @return True if this is a generic component.
     */
    bool
    isGeneric() const;

    /**
     * Check if this component is an ImplicitSha256Digest component.
     * @return True if this is an ImplicitSha256Digest component.
     */
    bool
    isImplicitSha256Digest() const;

    /**
     * Interpret the name component as a network-ordered number and return an integer.
     * @return The integer number.
     */
    uint64_t
    toNumber() const;

    /**
     * Interpret the name component as a network-ordered number with a marker
     * and return an integer.
     * @param marker The required first byte of the component.
     * @param result Return the integer number.
     * @return 0 for success, or an error code if the first byte of the component
     * does not equal the marker.
     */
    ndn_Error
    toNumberWithMarker(uint8_t marker, uint64_t& result) const;

    /**
     * Interpret the name component as a network-ordered number with a prefix
     * and return an integer.
     * @param prefix The required prefix of the component.
     * @param prefixLength The length of prefix.
     * @param result Return the integer number.
     * @return 0 for success, or an error code if the first bytes of the
     * component do not equal the prefix.
     */
    ndn_Error
    toNumberWithPrefix
      (const uint8_t* prefix, size_t prefixLength, uint64_t& result) const;

    /**
     * Check if this name component begins with the given prefix.
     * @param prefix The required prefix of the component.
     * @param prefixLength The length of prefix.
     * @return True if this component begins with the prefix.
     */
    bool
    hasPrefix(const uint8_t* prefix, size_t prefixLength) const;

    /**
     * Interpret this name component as a segment number according to NDN naming
     * conventions for "Segment number" (marker 0x00).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @param result Return the integer segment number.
     * @return 0 for success, or an error code if the first byte of the
     * component is not the expected marker.
     */
    ndn_Error
    toSegment(uint64_t& result) const;

    /**
     * Interpret this name component as a segment byte offset according to NDN
     * naming conventions for segment "Byte offset" (marker 0xFB).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @param result Return the integer segment byte offset.
     * @return 0 for success, or an error code if the first byte of the
     * component is not the expected marker.
     */
    ndn_Error
    toSegmentOffset(uint64_t& result) const;

    /**
     * Interpret this name component as a version number according to NDN naming
     * conventions for "Versioning" (marker 0xFD). Note that this returns
     * the exact number from the component without converting it to a time
     * representation.
     * @param result Return the integer version number.
     * @return 0 for success, or an error code if the first byte of the
     * component is not the expected marker.
     */
    ndn_Error
    toVersion(uint64_t& result) const;

    /**
     * Interpret this name component as a timestamp  according to NDN naming
     * conventions for "Timestamp" (marker 0xFC).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @param result Return the number of microseconds since the UNIX epoch (Thursday,
     * 1 January 1970) not counting leap seconds.
     * @return 0 for success, or an error code if the first byte of the
     * component is not the expected marker.
     */
    ndn_Error
    toTimestamp(uint64_t& result) const;

    /**
     * Interpret this name component as a sequence number according to NDN naming
     * conventions for "Sequencing" (marker 0xFE).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @param result Return the integer sequence number.
     * @return 0 for success, or an error code if the first byte of the
     * component is not the expected marker.
     */
    ndn_Error
    toSequenceNumber(uint64_t& result) const;

    /**
     * Check if this is the same component as other.
     * @param other The other name component to check.
     * @return True if the components are equal, false if not.
     */
    bool
    equals(const Component& other) const;

    /**
     * Compare this component to the other component using NDN component ordering.
     * A component is less if it is shorter, otherwise if equal length do a byte
     * comparison.
     * @param other The other name component to compare with.
     * @return -1 if self is less than other, 1 if greater or 0 if equal.
     */
    int
    compare(const Component& other) const;

    /**
     * Set this name component to have a value which is the nonNegativeInteger
     * encoding of the number.
     * Also set the type to ndn_NameComponentType_GENERIC.
     * @param number The number to be encoded.
     * @param buffer The allocated buffer to hold the name component value. This
     * buffer must remain valid during the life of the name component. It is the
     * caller's responsibility to free this buffer if necessary when finished
     * with it.
     * @param bufferLength The number of bytes in the allocated buffer. This
     * should be at least 8 bytes to hold a 64-bit value.
     * @return 0 for success, or an error code if bufferLength is too small.
     */
    ndn_Error
    setFromNumber(uint64_t number, uint8_t* buffer, size_t bufferLength);

    /**
     * Set this name component to have a value which is the marker appended with
     * the nonNegativeInteger encoding of the number.
     * Also set the type to ndn_NameComponentType_GENERIC.
     * @param number The number to be encoded.
     * @param marker The marker to use as the first byte of the component.
     * @param buffer The allocated buffer to hold the name component value. This
     * buffer must remain valid during the life of the name component. It is the
     * caller's responsibility to free this buffer if necessary when finished
     * with it.
     * @param bufferLength The number of bytes in the allocated buffer. This
     * should be at least 9 bytes to hold a marker plus a 64-bit value.
     * @return 0 for success, or an error code if bufferLength is too small.
     */
    ndn_Error
    setFromNumberWithMarker
      (uint64_t number, uint8_t marker, uint8_t* buffer, size_t bufferLength);

    /**
     * Set this name component to have the encoded segment number according to
     * NDN naming conventions for "Segment number" (marker 0x00).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @param segment The segment number.
     * @param buffer The allocated buffer to hold the name component value. This
     * buffer must remain valid during the life of the name component. It is the
     * caller's responsibility to free this buffer if necessary when finished
     * with it.
     * @param bufferLength The number of bytes in the allocated buffer. This
     * should be at least 9 bytes to hold a marker plus a 64-bit value.
     * @return 0 for success, or an error code if bufferLength is too small.
     */
    ndn_Error
    setSegment(uint64_t segment, uint8_t* buffer, size_t bufferLength);

    /**
     * Set this name component to have the encoded segment byte offset according
     * to NDN naming conventions for segment "Byte offset" (marker 0xFB).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @param segmentOffset The segment byte offset.
     * @param buffer The allocated buffer to hold the name component value. This
     * buffer must remain valid during the life of the name component. It is the
     * caller's responsibility to free this buffer if necessary when finished
     * with it.
     * @param bufferLength The number of bytes in the allocated buffer. This
     * should be at least 9 bytes to hold a marker plus a 64-bit value.
     * @return 0 for success, or an error code if bufferLength is too small.
     */
    ndn_Error
    setSegmentOffset
      (uint64_t segmentOffset, uint8_t* buffer, size_t bufferLength);

    /**
     * Set this name component to have the encoded version number according to
     * NDN naming conventions for "Versioning" (marker 0xFD).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * Note that this encodes the exact value of version without converting from
     * a time representation.
     * @param version The version number.
     * @param buffer The allocated buffer to hold the name component value. This
     * buffer must remain valid during the life of the name component. It is the
     * caller's responsibility to free this buffer if necessary when finished
     * with it.
     * @param bufferLength The number of bytes in the allocated buffer. This
     * should be at least 9 bytes to hold a marker plus a 64-bit value.
     * @return 0 for success, or an error code if bufferLength is too small.
     */
    ndn_Error
    setVersion(uint64_t version, uint8_t* buffer, size_t bufferLength);

    /**
     * Set this name component to have the encoded timestamp according to NDN
     * naming conventions for "Timestamp" (marker 0xFC).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @param timestamp The number of microseconds since the UNIX epoch
     * (Thursday, 1 January 1970) not counting leap seconds.
     * @param buffer The allocated buffer to hold the name component value. This
     * buffer must remain valid during the life of the name component. It is the
     * caller's responsibility to free this buffer if necessary when finished
     * with it.
     * @param bufferLength The number of bytes in the allocated buffer. This
     * should be at least 9 bytes to hold a marker plus a 64-bit value.
     * @return 0 for success, or an error code if bufferLength is too small.
     */
    ndn_Error
    setTimestamp(uint64_t timestamp, uint8_t* buffer, size_t bufferLength);

    /**
     * Set this name component to have the encoded sequence number according to
     * NDN naming conventions for "Sequencing" (marker 0xFE).
     * http://named-data.net/doc/tech-memos/naming-conventions.pdf
     * @param sequenceNumber The sequence number.
     * @param buffer The allocated buffer to hold the name component value. This
     * buffer must remain valid during the life of the name component. It is the
     * caller's responsibility to free this buffer if necessary when finished
     * with it.
     * @param bufferLength The number of bytes in the allocated buffer. This
     * should be at least 9 bytes to hold a marker plus a 64-bit value.
     * @return 0 for success, or an error code if bufferLength is too small.
     */
    ndn_Error
    setSequenceNumber
      (uint64_t sequenceNumber, uint8_t* buffer, size_t bufferLength);

    /**
     * Set this name component to have type ImplicitSha256DigestComponent with
     * the given digest value, so that isImplicitSha256Digest() is true.
     * @param digest The pre-allocated buffer for the SHA-256 digest value.
     * @param digestLength The length of digest, which must be
     * ndn_SHA256_DIGEST_SIZE.
     * @return 0 for success, or an error code if digestLength is not
     * ndn_SHA256_DIGEST_SIZE.
     */
    ndn_Error
    setImplicitSha256Digest(const uint8_t* digest, size_t digestLength);

    /**
     * Set this name component to have type ImplicitSha256DigestComponent with
     * the given digest value, so that isImplicitSha256Digest() is true.
     * @param digest The BlobLite with the pre-allocated buffer for the SHA-256
     * digest value. Its size must be ndn_SHA256_DIGEST_SIZE.
     * @return 0 for success, or an error code if digest.size() is not
     * ndn_SHA256_DIGEST_SIZE.
     */
    ndn_Error
    setImplicitSha256Digest(const BlobLite& digest)
    {
      return setImplicitSha256Digest(digest.buf(), digest.size());
    }

    /**
     * Downcast the reference to the ndn_NameComponent struct to a NameLite::Component.
     * @param component A reference to the ndn_NameComponent struct.
     * @return The same reference as NameLite::Component.
     */
    static Component&
    downCast(ndn_NameComponent& component) { return *(Component*)&component; }

    static const Component&
    downCast(const ndn_NameComponent& component) { return *(Component*)&component; }

  private:
    friend class NameLite;
    friend class ExcludeLite;
  };

  /**
   * Create a NameLite to use the components array.
   * @param components the pre-allocated array of ndn_NameComponent. Instead of
   * an array of NameLite::Component, this is an array of the underlying
   * ndn_NameComponent struct so that it doesn't run the default constructor
   * unnecessarily.
   * @param maxComponents the number of elements in the allocated components array.
   */
  NameLite(ndn_NameComponent* components, size_t maxComponents);

  /**
   * Get the component at the given index. This does not check if i is out of bounds.
   * @param i The index of the component, starting from 0.  However, if i is
   * negative, return the component at size() - (-i).
   * @return The name component at the index.
   */
  const Component&
  get(int i) const
  {
    // TODO: Range check.
    if (i >= 0)
      return Component::downCast(components[i]);
    else
      // Negative index.
      return Component::downCast(components[nComponents - (-i)]);
  }

  /**
   * Get the number of components.
   * @return The number of components.
   */
  size_t
  size() const { return nComponents; }

  /**
   * Check if this name has the same component count and components as the given
   * name.
   * @param name The Name to check.
   * @return true if this matches the given name, otherwise false. This always
   * returns true if this name is empty.
   */
  bool
  equals(const NameLite& name);

  /**
   * Check if the N components of this name are the same as the first N
   * components of the given name.
   * @param name The Name to check.
   * @return true if this matches the given name, otherwise false.
   * This always returns true if this name is empty.
   */
  bool
  match(const NameLite& name) const;

  /**
   * Clear all the components.
   */
  void
  clear();

  /**
   * Append a GENERIC component to this name with the bytes in the given buffer.
   * @param value A pointer to the buffer with the bytes of the component.
   * This does not copy the bytes.
   * @param valueLength The number of bytes in value.
   * @return 0 for success, or an error code if there is no more room in the
   * components array.
   */
  ndn_Error
  append(const uint8_t* value, size_t valueLength);

  /**
   * Append a GENERIC component to this name with the bytes in the given blob.
   * @param value A BlobLite with the bytes of the component.  This does not
   * copy the bytes.
   * @return 0 for success, or an error code if there is no more room in the
   * components array.
   */
  ndn_Error
  append(const BlobLite& value) { return append(value.buf(), value.size()); }

  /**
   * Append a component to this name with the bytes in the given
   * component's value and the component's type.
   * @param component A Component with the bytes of the component value. This
   * does not copy the bytes.
   * @return 0 for success, or an error code if there is no more room in the
   * components array.
   */
  ndn_Error
  append(const Component& component);

  /**
   * Append all of the components of the given name to this name, including each
   * component's type.
   * @param name The NameLite with the components to copy.
   * @return 0 for success, or an error code if there is no more room in the
   * components array.
   */
  ndn_Error
  append(const NameLite& name);

  /**
   * Append a GENERIC component to this name with the bytes in raw string value.
   * @param value The null-terminated string, treated as a byte array.  This
   * does not copy the bytes.
   * @return 0 for success, or an error code if there is no more room in the
   * components array (nComponents is already maxComponents).
   */
  ndn_Error
  append(const char *value);

  /**
   * Append a component with the encoded segment number according to NDN
   * naming conventions for "Segment number" (marker 0x00).
   * http://named-data.net/doc/tech-memos/naming-conventions.pdf
   * @param segment The segment number.
   * @param buffer The allocated buffer to hold the name component value. This
   * buffer must remain valid during the life of the name component. It is the
   * caller's responsibility to free this buffer if necessary when finished with it.
   * @param bufferLength The number of bytes in the allocated buffer. This
   * should be at least 9 bytes to hold a marker plus a 64-bit value.
   * @return 0 for success, or an error code if there is no more room in the
   * components array or if bufferLength is too small.
   */
  ndn_Error
  appendSegment(uint64_t segment, uint8_t* buffer, size_t bufferLength);

  /**
   * Append a component with the encoded segment byte offset according to NDN
   * naming conventions for segment "Byte offset" (marker 0xFB).
   * http://named-data.net/doc/tech-memos/naming-conventions.pdf
   * @param segmentOffset The segment byte offset.
   * @param buffer The allocated buffer to hold the name component value. This
   * buffer must remain valid during the life of the name component. It is the
   * caller's responsibility to free this buffer if necessary when finished with it.
   * @param bufferLength The number of bytes in the allocated buffer. This
   * should be at least 9 bytes to hold a marker plus a 64-bit value.
   * @return 0 for success, or an error code if there is no more room in the
   * components array or if bufferLength is too small.
   */
  ndn_Error
  appendSegmentOffset
    (uint64_t segmentOffset, uint8_t* buffer, size_t bufferLength);

  /**
   * Append a component with the encoded version number according to NDN
   * naming conventions for "Versioning" (marker 0xFD).
   * http://named-data.net/doc/tech-memos/naming-conventions.pdf
   * Note that this encodes the exact value of version without converting from a time representation.
   * @param version The version number.
   * @param buffer The allocated buffer to hold the name component value. This
   * buffer must remain valid during the life of the name component. It is the
   * caller's responsibility to free this buffer if necessary when finished with it.
   * @param bufferLength The number of bytes in the allocated buffer. This
   * should be at least 9 bytes to hold a marker plus a 64-bit value.
   * @return 0 for success, or an error code if there is no more room in the
   * components array or if bufferLength is too small.
   */
  ndn_Error
  appendVersion(uint64_t version, uint8_t* buffer, size_t bufferLength);

  /**
   * Append a component with the encoded timestamp according to NDN naming
   * conventions for "Timestamp" (marker 0xFC).
   * http://named-data.net/doc/tech-memos/naming-conventions.pdf
   * @param timestamp The number of microseconds since the UNIX epoch (Thursday,
   * 1 January 1970) not counting leap seconds.
   * @param buffer The allocated buffer to hold the name component value. This
   * buffer must remain valid during the life of the name component. It is the
   * caller's responsibility to free this buffer if necessary when finished with it.
   * @param bufferLength The number of bytes in the allocated buffer. This
   * should be at least 9 bytes to hold a marker plus a 64-bit value.
   * @return 0 for success, or an error code if there is no more room in the
   * components array or if bufferLength is too small.
   */
  ndn_Error
  appendTimestamp(uint64_t timestamp, uint8_t* buffer, size_t bufferLength);

  /**
   * Append a component with the encoded sequence number according to NDN naming
   * conventions for "Sequencing" (marker 0xFE).
   * http://named-data.net/doc/tech-memos/naming-conventions.pdf
   * @param sequenceNumber The sequence number.
   * @param buffer The allocated buffer to hold the name component value. This
   * buffer must remain valid during the life of the name component. It is the
   * caller's responsibility to free this buffer if necessary when finished with it.
   * @param bufferLength The number of bytes in the allocated buffer. This
   * should be at least 9 bytes to hold a marker plus a 64-bit value.
   * @return 0 for success, or an error code if there is no more room in the
   * components array or if bufferLength is too small.
   */
  ndn_Error
  appendSequenceNumber
    (uint64_t sequenceNumber, uint8_t* buffer, size_t bufferLength);

  /**
   * Append a component of type ImplicitSha256DigestComponent to this name with
   * the given digest value, so that isImplicitSha256Digest() is true.
   * @param sequenceNumber The sequence number.
   * @param digest The pre-allocated buffer for the SHA-256 digest value.
   * @param digestLength The length of digest, which must be ndn_SHA256_DIGEST_SIZE.
   * @return 0 for success, or an error code if digestLength is not
   * ndn_SHA256_DIGEST_SIZE, or if there is no more room in the components array
   * (nComponents is already maxComponents).
   */
  ndn_Error
  appendImplicitSha256Digest(const uint8_t* digest, size_t digestLength);

  /**
   * Append a component of type ImplicitSha256DigestComponent to this name with
   * the given digest value, so that isImplicitSha256Digest() is true.
   * @param sequenceNumber The sequence number.
   * @param digest The pre-allocated SHA-256 digest value, whose size must be
   * ndn_SHA256_DIGEST_SIZE.
   * @return 0 for success, or an error code if digestLength is not
   * ndn_SHA256_DIGEST_SIZE, or if there is no more room in the components array
   * (nComponents is already maxComponents).
   */
  ndn_Error
  appendImplicitSha256Digest(const BlobLite& digest)
  {
    return appendImplicitSha256Digest(digest.buf(), digest.size());
  }

  /**
   * Set this name to have the values from the other name.
   * @param other The other NameLite to get values from.
   * @return 0 for success, or an error code if there is not enough room in this
   * object's components array.
   */
  ndn_Error
  set(const NameLite& other);

  /**
   * Remove and return the last name component.
   * @return A pointer to the last name component that was removed. If size() is
   * already zero then return a null pointer.
   */
  const Component*
  pop();

  /**
   * Downcast the reference to the ndn_Name struct to a NameLite.
   * @param name A reference to the ndn_Name struct.
   * @return The same reference as NameLite.
   */
  static NameLite&
  downCast(ndn_Name& name) { return *(NameLite*)&name; }

  static const NameLite&
  downCast(const ndn_Name& name) { return *(NameLite*)&name; }

private:
  // Declare friends who can downcast to the private base.
  friend class Tlv0_2WireFormatLite;

  /**
   * Don't allow the copy constructor. Instead use set(const NameLite&) which
   * can return an error if there is no more room in the name components array.
   */
  NameLite(const NameLite& other);

  /**
   * Don't allow the assignment operator. Instead use set(const NameLite&) which
   * can return an error if there is no more room in the name components array.
   */
  NameLite& operator=(const NameLite& other);
};

}

#endif
