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

#ifndef NDN_EXCLUDE_LITE_HPP
#define NDN_EXCLUDE_LITE_HPP

#include "name-lite.hpp"
#include "../c/interest-types.h"

namespace ndn {

/**
 * An ExcludeLite holds an array of ExcludeLite::Entry.
 */
class ExcludeLite : private ndn_Exclude {
public:
  /**
   * Create an ExcludeLite with the entries array.
   * @param entries The pre-allocated array of ndn_ExcludeEntry. Instead
   * of an array of ExcludeLite::Entry, this is an array of the underlying
   * ndn_ExcludeEntry struct so that it doesn't run the default constructor
   * unnecessarily.
   * @param maxEntries The number of elements in the allocated entries array.
   */
  ExcludeLite(ndn_ExcludeEntry* entries, size_t maxEntries);

  class Entry : private ndn_ExcludeEntry {
  public:
    /**
     * Create an ExcludeLite::Entry of type ndn_Exclude_ANY.
     */
    Entry();

    /**
     * Create an ExcludeLite::Entry of type ndn_Exclude_COMPONENT with the given
     * component value.
     * @param component The pre-allocated buffer for the component value.
     * @param componentLength the number of bytes in value.
     */
    Entry(const uint8_t *component, size_t componentLength);

    /**
     * Create an ExcludeLite::Entry of type ndn_Exclude_COMPONENT, taking the
     * component pointer and size from the BlobLite value.
     * @param component The BlobLite with the pointer to use for the component.
     */
    Entry(const BlobLite& component);

    ndn_ExcludeType
    getType() const { return type; }

    const NameLite::Component&
    getComponent() const { return NameLite::Component::downCast(component); }

    /**
     * Downcast the reference to the ndn_ExcludeEntry struct to an ExcludeLite::Entry.
     * @param entry A reference to the ndn_ExcludeEntry struct.
     * @return The same reference as ExcludeLite::Entry.
     */
    static Entry&
    downCast(ndn_ExcludeEntry& entry) { return *(Entry*)&entry; }

    static const Entry&
    downCast(const ndn_ExcludeEntry& entry) { return *(Entry*)&entry; }
  };

  /**
   * Get the number of entries.
   * @return The number of entries.
   */
  size_t
  size() const { return nEntries; }

  /**
   * Get the entry at the given index.
   * @param i The index of the entry, starting from 0.
   * @return The entry at the index.
   */
  const ExcludeLite::Entry&
  get(size_t i) const
  {
    // TODO: Range check.
    return Entry::downCast(entries[i]);
  }

  /**
   * Append a new entry of type ndn_Exclude_ANY.
   * @return 0 for success, or an error code if there is no more room in the
   * entries array (nEntries is already maxEntries).
   */
  ndn_Error
  appendAny();

  /**
   * Append a new entry of type ndn_Exclude_COMPONENT with the given GENERIC
   * component value.
   * @param component The bytes of the component.  This does not copy the bytes.
   * @param componentLength The number of bytes in component.
   * @return 0 for success, or an error code if there is no more room in the
   * entries array (nEntries is already maxEntries).
   */
  ndn_Error
  appendComponent(const uint8_t* component, size_t componentLength);

  /**
   * Append a new entry of type ndn_Exclude_COMPONENT with the given component
   * value.
   * @param component A NameList::Component for the exclude value. This does not
   * copy the bytes.
   * @return 0 for success, or an error code if there is no more room in the
   * entries array (nEntries is already maxEntries).
   */
  ndn_Error
  appendComponent(const NameLite::Component& component);

  /**
   * Clear all the entries.
   */
  void
  clear();

  /**
   * Set this exclude object to have the values from the other exclude.
   * @param other The other ExcludeLite to get values from.
   * @return 0 for success, or an error code if there is not enough room in this
   * object's entries array.
   */
  ndn_Error
  set(const ExcludeLite& other);

  /**
   * Downcast the reference to the ndn_Exclude struct to an ExcludeLite.
   * @param exclude A reference to the ndn_Exclude struct.
   * @return The same reference as ExcludeLite.
   */
  static ExcludeLite&
  downCast(ndn_Exclude& exclude) { return *(ExcludeLite*)&exclude; }

  static const ExcludeLite&
  downCast(const ndn_Exclude& exclude) { return *(ExcludeLite*)&exclude; }

private:
  /**
   * Don't allow the copy constructor. Instead use set(const ExcludeLite&)
   * which can return an error if there is no more room in the entries array.
   */
  ExcludeLite(const ExcludeLite& other);

  /**
   * Don't allow the assignment operator. Instead use set(const ExcludeLite&)
   * which can return an error if there is no more room in the entries array.
   */
  ExcludeLite& operator=(const ExcludeLite& other);
};

}

#endif
