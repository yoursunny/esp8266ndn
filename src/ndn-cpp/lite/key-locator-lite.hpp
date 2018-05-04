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

#ifndef NDN_KEY_LOCATOR_LITE_HPP
#define NDN_KEY_LOCATOR_LITE_HPP

#include "name-lite.hpp"
#include "../c/key-types.h"

namespace ndn {

/**
 * A KeyLocatorLite holds a type and other info to represent the key which signs
 * a Data packet.
 */
class KeyLocatorLite : private ndn_KeyLocator {
public:
  /**
   * Create a KeyLocatorLite with the pre-allocated nameComponents, and defaults
   * for all the values.
   * @param keyNameComponents The pre-allocated array of ndn_NameComponent.
   * Instead of an array of NameLite::Component, this is an array of the underlying
   * ndn_NameComponent struct so that it doesn't run the default constructor
   * unnecessarily.
   * @param maxKeyNameComponents The number of elements in the allocated
   * keyNameComponents array.
   */
  KeyLocatorLite(ndn_NameComponent* keyNameComponents, size_t maxKeyNameComponents);

  ndn_KeyLocatorType
  getType() const { return type; }

  const BlobLite&
  getKeyData() const { return BlobLite::downCast(keyData); }

  NameLite&
  getKeyName() { return NameLite::downCast(keyName); }

  const NameLite&
  getKeyName() const { return NameLite::downCast(keyName); }

  /**
   * Set this object's key name to have the values from the given keyName.
   * @param keyName The key name to get values from.
   * @return 0 for success, or an error code if there is not enough room in this
   * object's keyName components array.
   */
  ndn_Error
  setKeyName(const NameLite& keyName)
  {
    return NameLite::downCast(this->keyName).set(keyName);
  }

  void
  setType(ndn_KeyLocatorType type) { this->type = type; }

  /**
   * Set the key data.
   * @param keyData The key data. This does not copy the bytes of the blob.
   */
  void
  setKeyData(const BlobLite& keyData)
  {
    BlobLite::downCast(this->keyData) = keyData;
  }

  /**
   * Set this key locator to have the values from the other key locator.
   * @param other The other KeyLocatorLite to get values from.
   * @return 0 for success, or an error code if there is not enough room in this
   * object's keyName components array.
   */
  ndn_Error
  set(const KeyLocatorLite& other);

  /**
   * Downcast the reference to the ndn_KeyLocator struct to a KeyLocatorLite.
   * @param keyLocator A reference to the ndn_KeyLocator struct.
   * @return The same reference as KeyLocatorLite.
   */
  static KeyLocatorLite&
  downCast(ndn_KeyLocator& keyLocator) { return *(KeyLocatorLite*)&keyLocator; }

  static const KeyLocatorLite&
  downCast(const ndn_KeyLocator& keyLocator) { return *(KeyLocatorLite*)&keyLocator; }

private:
  /**
   * Don't allow the copy constructor. Instead use set(const KeyLocatorLite&)
   * which can return an error if there is no more room in the name components
   * array.
   */
  KeyLocatorLite(const KeyLocatorLite& other);

  /**
   * Don't allow the assignment operator. Instead use set(const KeyLocatorLite&)
   * which can return an error if there is no more room in the name components
   * array.
   */
  KeyLocatorLite& operator=(const KeyLocatorLite& other);
};

}

#endif
