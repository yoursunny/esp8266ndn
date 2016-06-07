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

#include "../c/interest.h"
#include <ndn-cpp/lite/exclude-lite.hpp>

namespace ndn {

ExcludeLite::Entry::Entry()
{
  ndn_ExcludeEntry_initialize(this, ndn_Exclude_ANY, 0, 0);
}

ExcludeLite::Entry::Entry(const uint8_t *component, size_t componentLength)
{
  ndn_ExcludeEntry_initialize
    (this, ndn_Exclude_COMPONENT, component, componentLength);
}

ExcludeLite::Entry::Entry(const BlobLite& component)
{
  ndn_ExcludeEntry_initialize
    (this, ndn_Exclude_COMPONENT, component.buf(), component.size());
}

ExcludeLite::ExcludeLite(ndn_ExcludeEntry* entries, size_t maxEntries)
{
  ndn_Exclude_initialize(this, entries, maxEntries);
}

ndn_Error
ExcludeLite::appendAny()
{
  return ndn_Exclude_appendAny(this);
}

ndn_Error
ExcludeLite::appendComponent(const uint8_t* component, size_t componentLength)
{
  return ndn_Exclude_appendComponent(this, component, componentLength);
}

void
ExcludeLite::clear() { ndn_Exclude_clear(this); }

ndn_Error
ExcludeLite::set(const ExcludeLite& other)
{
  return ndn_Exclude_setFromExclude(this, &other);
}

}
