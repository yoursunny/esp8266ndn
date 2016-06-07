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

#include "../c/name.h"
#include <ndn-cpp/lite/name-lite.hpp>

namespace ndn {

NameLite::Component::Component()
{
  ndn_NameComponent_initialize(this, 0, 0);
}

NameLite::Component::Component(const uint8_t* value, size_t valueLength)
{
  ndn_NameComponent_initialize(this, value, valueLength);
}

NameLite::Component::Component(const BlobLite& value)
{
  ndn_NameComponent_initialize(this, value.buf(), value.size());
}

bool
NameLite::Component::isSegment() const
{
  return ndn_NameComponent_isSegment(this) != 0;
}

bool
NameLite::Component::isSegmentOffset() const
{
  return ndn_NameComponent_isSegmentOffset(this) != 0;
}

bool
NameLite::Component::isVersion() const
{
  return ndn_NameComponent_isVersion(this) != 0;
}

bool
NameLite::Component::isTimestamp() const
{
  return ndn_NameComponent_isTimestamp(this) != 0;
}

bool
NameLite::Component::isSequenceNumber() const
{
  return ndn_NameComponent_isSequenceNumber(this) != 0;
}

uint64_t
NameLite::Component::toNumber() const
{
  return ndn_NameComponent_toNumber(this);
}

ndn_Error
NameLite::Component::toNumberWithMarker(uint8_t marker, uint64_t& result) const
{
  return ndn_NameComponent_toNumberWithMarker(this, marker, &result);
}

ndn_Error
NameLite::Component::toNumberWithPrefix
  (const uint8_t* prefix, size_t prefixLength, uint64_t& result) const
{
  return ndn_NameComponent_toNumberWithPrefix
    (this, prefix, prefixLength, &result);
}

bool
NameLite::Component::hasPrefix(const uint8_t* prefix, size_t prefixLength) const
{
  return ndn_NameComponent_hasPrefix(this, prefix, prefixLength) != 0;
}

ndn_Error
NameLite::Component::toSegment(uint64_t& result) const
{
  return ndn_NameComponent_toSegment(this, &result);
}

ndn_Error
NameLite::Component::toSegmentOffset(uint64_t& result) const
{
  return ndn_NameComponent_toSegmentOffset(this, &result);
}

ndn_Error
NameLite::Component::toVersion(uint64_t& result) const
{
  return ndn_NameComponent_toVersion(this, &result);
}

ndn_Error
NameLite::Component::toTimestamp(uint64_t& result) const
{
  return ndn_NameComponent_toTimestamp(this, &result);
}

ndn_Error
NameLite::Component::toSequenceNumber(uint64_t& result) const
{
  return ndn_NameComponent_toSequenceNumber(this, &result);
}

int
NameLite::Component::compare(const NameLite::Component& other) const
{
  return ndn_NameComponent_compare(this, &other);
}

NameLite::NameLite(ndn_NameComponent* components, size_t maxComponents)
{
  ndn_Name_initialize(this, components, maxComponents);
}

bool
NameLite::equals(const NameLite& name)
{
  return ndn_Name_equals(this, &name) != 0;
}

bool
NameLite::match(const NameLite& name) const
{
  return ndn_Name_match(this, &name) != 0;
}

void
NameLite::clear() { ndn_Name_clear(this); }

ndn_Error
NameLite::append(const uint8_t* value, size_t valueLength)
{
  return ndn_Name_appendComponent(this, value, valueLength);
}

ndn_Error
NameLite::append(const char *value) { return ndn_Name_appendString(this, value); }

ndn_Error
NameLite::appendSegment(uint64_t segment, uint8_t* buffer, size_t bufferLength)
{
  ndn_Error error;
  if ((error = ndn_Name_appendComponent(this, 0, 0)))
    return error;
  return ndn_NameComponent_setSegment
    (&components[nComponents - 1], segment, buffer, bufferLength);
}

ndn_Error
NameLite::appendSegmentOffset
  (uint64_t segmentOffset, uint8_t* buffer, size_t bufferLength)
{
  ndn_Error error;
  if ((error = ndn_Name_appendComponent(this, 0, 0)))
    return error;
  return ndn_NameComponent_setSegmentOffset
    (&components[nComponents - 1], segmentOffset, buffer, bufferLength);
}

ndn_Error
NameLite::appendVersion(uint64_t version, uint8_t* buffer, size_t bufferLength)
{
  ndn_Error error;
  if ((error = ndn_Name_appendComponent(this, 0, 0)))
    return error;
  return ndn_NameComponent_setVersion
    (&components[nComponents - 1], version, buffer, bufferLength);
}

ndn_Error
NameLite::appendTimestamp(uint64_t timestamp, uint8_t* buffer, size_t bufferLength)
{
  ndn_Error error;
  if ((error = ndn_Name_appendComponent(this, 0, 0)))
    return error;
  return ndn_NameComponent_setTimestamp
    (&components[nComponents - 1], timestamp, buffer, bufferLength);
}

ndn_Error
NameLite::appendSequenceNumber
  (uint64_t sequenceNumber, uint8_t* buffer, size_t bufferLength)
{
  ndn_Error error;
  if ((error = ndn_Name_appendComponent(this, 0, 0)))
    return error;
  return ndn_NameComponent_setSequenceNumber
    (&components[nComponents - 1], sequenceNumber, buffer, bufferLength);
}

ndn_Error
NameLite::set(const NameLite& other) { return ndn_Name_setFromName(this, &other); }

}
