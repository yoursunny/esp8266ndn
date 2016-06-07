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

#ifndef NDN_NAME_H
#define NDN_NAME_H

#include <ndn-cpp/c/name-types.h>
#include <ndn-cpp/c/errors.h>
#include "util/blob.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @param self pointer to the ndn_NameComponent struct
 * @param value the pre-allocated buffer for the component value
 * @param valueLength the number of bytes in value
 */
static __inline void ndn_NameComponent_initialize(struct ndn_NameComponent *self, const uint8_t *value, size_t valueLength)
{
  ndn_Blob_initialize(&self->value, value, valueLength);
}

/**
 * Check if this component is a segment number according to NDN naming
 * conventions for "Segment number" (marker 0x00) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @return 1 if this is a segment number, else 0.
 */
static __inline int
ndn_NameComponent_isSegment(const struct ndn_NameComponent *self)
{
  return self->value.length >= 1 && self->value.value[0] == 0x00 ? 1 : 0;
}

/**
 * Check if this component is a segment byte offset according to NDN naming
 * conventions for "Byte offset" (marker 0xFB) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @return 1 if this is a segment byte offset, else 0.
 */
static __inline int
ndn_NameComponent_isSegmentOffset(const struct ndn_NameComponent *self)
{
  return self->value.length >= 1 && self->value.value[0] == 0xFB ? 1 : 0;
}

/**
 * Check if this component is a version number according to NDN naming
 * conventions for "Versioning" (marker 0xFD) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @return 1 if this is a version number, else 0.
 */
static __inline int
ndn_NameComponent_isVersion(const struct ndn_NameComponent *self)
{
  return self->value.length >= 1 && self->value.value[0] == 0xFD ? 1 : 0;
}

/**
 * Check if this component is a timestamp according to NDN naming
 * conventions for "Timestamp" (marker 0xFC) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @return 1 if this is a timestamp, else 0.
 */
static __inline int
ndn_NameComponent_isTimestamp(const struct ndn_NameComponent *self)
{
  return self->value.length >= 1 && self->value.value[0] == 0xFC ? 1 : 0;
}

/**
 * Check if this component is a sequence number according to NDN naming
 * conventions for "Sequencing" (marker 0xFE) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @return 1 if this is a sequence number, else 0.
 */
static __inline int
ndn_NameComponent_isSequenceNumber(const struct ndn_NameComponent *self)
{
  return self->value.length >= 1 && self->value.value[0] == 0xFE ? 1 : 0;
}

/**
 * Interpret the name component as a network-ordered number and return an integer.
 * @param self A pointer to the ndn_NameComponent struct.
 * @return The integer number.
 */
uint64_t ndn_NameComponent_toNumber(const struct ndn_NameComponent *self);

/**
 * Interpret the name component as a network-ordered number with a marker and return an integer.
 * @param self A pointer to the ndn_NameComponent struct.
 * @param marker The required first byte of the component.
 * @param result Return the integer number.
 * @return 0 for success, or an error code if the first byte of the component does not equal the marker.
 */
ndn_Error ndn_NameComponent_toNumberWithMarker
  (const struct ndn_NameComponent *self, uint8_t marker, uint64_t *result);

/**
 * Interpret this name component as a segment number according to NDN naming
 * conventions for "Segment number" (marker 0x00) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @param result Return the integer segment number.
 * @return 0 for success, or an error code if the first byte of the
 * component is not the expected marker.
 */
static __inline ndn_Error
ndn_NameComponent_toSegment
  (const struct ndn_NameComponent *self, uint64_t *result)
{
  return ndn_NameComponent_toNumberWithMarker(self, 0x00, result);
}

/**
 * Interpret this name component as a segment byte offset according to NDN naming
 * conventions for "Byte offset" (marker 0xFB) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @param result Return the integer segment byte offset.
 * @return 0 for success, or an error code if the first byte of the
 * component is not the expected marker.
 */
static __inline ndn_Error
ndn_NameComponent_toSegmentOffset
  (const struct ndn_NameComponent *self, uint64_t *result)
{
  return ndn_NameComponent_toNumberWithMarker(self, 0xFB, result);
}

/**
 * Interpret this name component as a version number according to NDN naming
 * conventions for "Versioning" (marker 0xFD) and return an integer. Note that
 * this returns the exact number from the component without converting it to a
 * time representation.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @param result Return the integer version number.
 * @return 0 for success, or an error code if the first byte of the
 * component is not the expected marker.
 */
static __inline ndn_Error
ndn_NameComponent_toVersion
  (const struct ndn_NameComponent *self, uint64_t *result)
{
  return ndn_NameComponent_toNumberWithMarker(self, 0xFD, result);
}

/**
 * Interpret this name component as a timestamp according to NDN naming
 * conventions for "Timestamp" (marker 0xFC) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @param result Return the number of microseconds since the UNIX epoch
 * (Thursday, 1 January 1970) not counting leap seconds.
 * @return 0 for success, or an error code if the first byte of the
 * component is not the expected marker.
 */
static __inline ndn_Error
ndn_NameComponent_toTimestamp
  (const struct ndn_NameComponent *self, uint64_t *result)
{
  return ndn_NameComponent_toNumberWithMarker(self, 0xFC, result);
}

/**
 * Interpret this name component as a sequence number according to NDN naming
 * conventions for "Sequencing" (marker 0xFE) and return an integer.
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to the ndn_NameComponent struct.
 * @param result Return the integer sequence number.
 * @return 0 for success, or an error code if the first byte of the
 * component is not the expected marker.
 */
static __inline ndn_Error
ndn_NameComponent_toSequenceNumber
  (const struct ndn_NameComponent *self, uint64_t *result)
{
  return ndn_NameComponent_toNumberWithMarker(self, 0xFE, result);
}

/**
 * Interpret the name component as a network-ordered number with a prefix and return an integer.
 * @param self A pointer to the ndn_NameComponent struct.
 * @param prefix The required prefix of the component.
 * @param prefixLength The length of prefix.
 * @param result Return the integer number.
 * @return 0 for success, or an error code if the first bytes of the component do not equal the prefix.
 */
ndn_Error ndn_NameComponent_toNumberWithPrefix
  (const struct ndn_NameComponent *self, const uint8_t *prefix,
   size_t prefixLength, uint64_t *result);

/**
 * Check if this name component begins with the given prefix.
 * @param self A pointer to the ndn_NameComponent struct.
 * @param prefix The required prefix of the component.
 * @param prefixLength The length of prefix.
 * @return 1 if this component begins with the prefix, 0 if not.
 */
int
ndn_NameComponent_hasPrefix
  (const struct ndn_NameComponent *self, const uint8_t *prefix,
   size_t prefixLength);

/**
 * Compare this component to the other component using NDN component ordering.
 * A component is less if it is shorter, otherwise if equal length do a byte
 * comparison.
 * @param self A pointer this name component.
 * @param other A pointer to the other name component to compare with.
 * @return -1 if self is less than other, 1 if greater or 0 if equal.
 */
int ndn_NameComponent_compare
  (const struct ndn_NameComponent *self, const struct ndn_NameComponent *other);

/**
 * Set this name component to have the values from the other name component.
 * @param self A pointer to this ndn_NameComponent struct.
 * @param other A pointer to the other ndn_NameComponent struct to get values from.
 */
static __inline void
ndn_NameComponent_setFromNameComponent
  (struct ndn_NameComponent *self, const struct ndn_NameComponent *other)
{
  *self = *other;
}

/**
 * Set this name component to have a value which is the nonNegativeInteger
 * encoding of the number.
 * @param self A pointer to this ndn_NameComponent struct.
 * @param number The number to be encoded.
 * @param buffer The allocated buffer to hold the name component value. This
 * buffer must remain valid during the life of the name component. It is the
 * caller's responsibility to free this buffer if necessary when finished with it.
 * @param bufferLength The number of bytes in the allocated buffer. This
 * should be at least 8 bytes to hold a 64-bit value.
 * @return 0 for success, or an error code if bufferLength is too small.
 */
ndn_Error
ndn_NameComponent_setFromNumber
  (struct ndn_NameComponent *self, uint64_t number, uint8_t *buffer,
   size_t bufferLength);

/**
 * Set this name component to have a value which is the marker appended with the
 * nonNegativeInteger encoding of the number.
 * @param self A pointer to this ndn_NameComponent struct.
 * @param number The number to be encoded.
 * @param marker The marker to use as the first byte of the component.
 * @param buffer The allocated buffer to hold the name component value. This
 * buffer must remain valid during the life of the name component. It is the
 * caller's responsibility to free this buffer if necessary when finished with it.
 * @param bufferLength The number of bytes in the allocated buffer. This
 * should be at least 9 bytes to hold a marker plus a 64-bit value.
 * @return 0 for success, or an error code if bufferLength is too small.
 */
ndn_Error
ndn_NameComponent_setFromNumberWithMarker
  (struct ndn_NameComponent *self, uint64_t number, uint8_t marker,
   uint8_t *buffer, size_t bufferLength);

/**
 * Set this name component to have the encoded segment number according to NDN
 * naming conventions for "Segment number" (marker 0x00).
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to this ndn_NameComponent struct.
 * @param segment The segment number.
 * @param buffer The allocated buffer to hold the name component value. This
 * buffer must remain valid during the life of the name component. It is the
 * caller's responsibility to free this buffer if necessary when finished with it.
 * @param bufferLength The number of bytes in the allocated buffer. This
 * should be at least 9 bytes to hold a marker plus a 64-bit value.
 * @return 0 for success, or an error code if bufferLength is too small.
 */
static __inline ndn_Error
ndn_NameComponent_setSegment
  (struct ndn_NameComponent *self, uint64_t segment, uint8_t* buffer,
   size_t bufferLength)
{
  return ndn_NameComponent_setFromNumberWithMarker
    (self, segment, 0x00, buffer, bufferLength);
}

/**
 * Set this name component to have the encoded segment byte offset according to NDN
 * naming conventions for segment "Byte offset" (marker 0xFB).
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to this ndn_NameComponent struct.
 * @param segmentOffset The segment byte offset.
 * @param buffer The allocated buffer to hold the name component value. This
 * buffer must remain valid during the life of the name component. It is the
 * caller's responsibility to free this buffer if necessary when finished with it.
 * @param bufferLength The number of bytes in the allocated buffer. This
 * should be at least 9 bytes to hold a marker plus a 64-bit value.
 * @return 0 for success, or an error code if bufferLength is too small.
 */
static __inline ndn_Error
ndn_NameComponent_setSegmentOffset
  (struct ndn_NameComponent *self, uint64_t segmentOffset, uint8_t* buffer,
   size_t bufferLength)
{
  return ndn_NameComponent_setFromNumberWithMarker
    (self, segmentOffset, 0xFB, buffer, bufferLength);
}

/**
 * Set this name component to have the encoded version number according to NDN
 * naming conventions for "Versioning" (marker 0xFD).
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * Note that this encodes the exact value of version without converting from a time representation.
 * @param self A pointer to this ndn_NameComponent struct.
 * @param version The version number.
 * @param buffer The allocated buffer to hold the name component value. This
 * buffer must remain valid during the life of the name component. It is the
 * caller's responsibility to free this buffer if necessary when finished with it.
 * @param bufferLength The number of bytes in the allocated buffer. This
 * should be at least 9 bytes to hold a marker plus a 64-bit value.
 * @return 0 for success, or an error code if bufferLength is too small.
 */
static __inline ndn_Error
ndn_NameComponent_setVersion
  (struct ndn_NameComponent *self, uint64_t version, uint8_t* buffer,
   size_t bufferLength)
{
  return ndn_NameComponent_setFromNumberWithMarker
    (self, version, 0xFD, buffer, bufferLength);
}

/**
 * Set this name component to have the encoded timestamp according to NDN naming
 * conventions for "Timestamp" (marker 0xFC).
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to this ndn_NameComponent struct.
 * @param timestamp The number of microseconds since the UNIX epoch (Thursday,
 * 1 January 1970) not counting leap seconds.
 * @param buffer The allocated buffer to hold the name component value. This
 * buffer must remain valid during the life of the name component. It is the
 * caller's responsibility to free this buffer if necessary when finished with it.
 * @param bufferLength The number of bytes in the allocated buffer. This
 * should be at least 9 bytes to hold a marker plus a 64-bit value.
 * @return 0 for success, or an error code if bufferLength is too small.
 */
static __inline ndn_Error
ndn_NameComponent_setTimestamp
  (struct ndn_NameComponent *self, uint64_t timestamp, uint8_t* buffer,
   size_t bufferLength)
{
  return ndn_NameComponent_setFromNumberWithMarker
    (self, timestamp, 0xFC, buffer, bufferLength);
}

/**
 * Set this name component to have the encoded sequence number according to NDN naming
 * conventions for "Sequencing" (marker 0xFE).
 * http://named-data.net/doc/tech-memos/naming-conventions.pdf
 * @param self A pointer to this ndn_NameComponent struct.
 * @param sequenceNumber The sequence number.
 * @param buffer The allocated buffer to hold the name component value. This
 * buffer must remain valid during the life of the name component. It is the
 * caller's responsibility to free this buffer if necessary when finished with it.
 * @param bufferLength The number of bytes in the allocated buffer. This
 * should be at least 9 bytes to hold a marker plus a 64-bit value.
 * @return 0 for success, or an error code if bufferLength is too small.
 */
static __inline ndn_Error
ndn_NameComponent_setSequenceNumber
  (struct ndn_NameComponent *self, uint64_t sequenceNumber, uint8_t* buffer,
   size_t bufferLength)
{
  return ndn_NameComponent_setFromNumberWithMarker
    (self, sequenceNumber, 0xFE, buffer, bufferLength);
}

/**
 * Initialize an ndn_Name struct with the components array.
 * @param self pointer to the ndn_Name struct
 * @param components the pre-allocated array of ndn_NameComponent
 * @param maxComponents the number of elements in the allocated components array
 */
static __inline void ndn_Name_initialize(struct ndn_Name *self, struct ndn_NameComponent *components, size_t maxComponents)
{
  self->components = components;
  self->maxComponents = maxComponents;
  self->nComponents = 0;
}

/**
 * Clear all the components.
 * @param self A pointer to the ndn_Name struct.
 */
static __inline void
ndn_Name_clear(struct ndn_Name *self) { self->nComponents = 0; }

/**
 * Check if this name has the same component count and components as the given
 * name.
 * @param self A pointer to this ndn_Name struct.
 * @param name A pointer to the other name to check.
 * @return 1 if this matches the given name, otherwise 0. This always returns 1
 * if this name is empty.
 */
int ndn_Name_equals(const struct ndn_Name *self, const struct ndn_Name *name);

/**
 * Return true if the N components of this name are the same as the first N components of the given name.
 * @param self A pointer to the ndn_Name struct.
 * @param name A pointer to the other name to match.
 * @return 1 if this matches the given name, 0 otherwise.  This always returns 1 if this name is empty.
 */
int ndn_Name_match(const struct ndn_Name *self, const struct ndn_Name *name);

/**
 * Append a component to this name with the bytes in the given array.
 * @param self pointer to the ndn_Name struct.
 * @param value The bytes of the component.  This does not copy the bytes.
 * @param valueLength The number of bytes in value.
 * @return 0 for success, or an error code if there is no more room in the components array (nComponents is already maxComponents).
 */
ndn_Error ndn_Name_appendComponent(struct ndn_Name *self, const uint8_t* value, size_t valueLength);

/**
 * Append a component to this name with the bytes in the given blob.
 * @param self pointer to the ndn_Name struct.
 * @param value An ndn_Blob with the bytes of the component.  This does not copy the bytes.
 * @return 0 for success, or an error code if there is no more room in the components array (nComponents is already maxComponents).
 */
static __inline ndn_Error ndn_Name_appendBlob(struct ndn_Name *self, struct ndn_Blob *value)
{
  return ndn_Name_appendComponent(self, value->value, value->length);
}

/**
 * Append a component to this name with the bytes in raw string value.
 * @param self pointer to the ndn_Name struct.
 * @param value The null-terminated string, treated as a byte array.  This does not copy the bytes.
 * @return 0 for success, or an error code if there is no more room in the components array (nComponents is already maxComponents).
 */
ndn_Error ndn_Name_appendString(struct ndn_Name *self, const char * value);

/**
 * Set this name to have the values from the other name.
 * @param self A pointer to the ndn_Name struct.
 * @param other A pointer to the other ndn_Name struct to get values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's components array.
 */
ndn_Error
ndn_Name_setFromName(struct ndn_Name *self, const struct ndn_Name *other);

#ifdef __cplusplus
}
#endif

#endif

