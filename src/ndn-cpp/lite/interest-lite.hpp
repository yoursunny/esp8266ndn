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

#ifndef NDN_INTEREST_LITE_HPP
#define NDN_INTEREST_LITE_HPP

#include "common-lite.hpp"
#include "exclude-lite.hpp"
#include "key-locator-lite.hpp"
#include "../c/interest-types.h"

namespace ndn {

/**
 * An InterestLite holds a NameLite and other fields for an interest.
 */
class InterestLite : private ndn_Interest {
public:
  /**
   * Create an InterestLite with the pre-allocated nameComponents and
   * excludeEntries, and defaults for all the values.
   * @param nameComponents The pre-allocated array of ndn_NameComponent. Instead
   * of an array of NameLite::Component, this is an array of the underlying
   * ndn_NameComponent struct so that it doesn't run the default constructor
   * unnecessarily.
   * @param maxNameComponents The number of elements in the allocated
   * nameComponents array.
   * @param excludeEntries The pre-allocated array of ndn_ExcludeEntry. Instead
   * of an array of ExcludeLite::Entry, this is an array of the underlying
   * ndn_ExcludeEntry struct so that it doesn't run the default constructor
   * unnecessarily.
   * @param maxExcludeEntries The number of elements in the allocated
   * excludeEntries array.
   * @param keyNameComponents The pre-allocated array of ndn_NameComponent for
   * the keyLocator. Instead of an array of NameLite::Component, this is an
   * array of the underlying ndn_NameComponent struct so that it doesn't run the
   * default constructor unnecessarily.
   * @param maxKeyNameComponents The number of elements in the allocated
   * keyNameComponents array.
   */
  InterestLite
    (ndn_NameComponent *nameComponents, size_t maxNameComponents,
     ndn_ExcludeEntry *excludeEntries, size_t maxExcludeEntries,
     ndn_NameComponent *keyNameComponents, size_t maxKeyNameComponents);

  NameLite&
  getName() { return NameLite::downCast(name); }

  const NameLite&
  getName() const { return NameLite::downCast(name); }

  int
  getMinSuffixComponents() const { return minSuffixComponents; }

  int
  getMaxSuffixComponents() const { return maxSuffixComponents; }

  const KeyLocatorLite&
  getKeyLocator() const { return KeyLocatorLite::downCast(keyLocator); }

  KeyLocatorLite&
  getKeyLocator() { return KeyLocatorLite::downCast(keyLocator); }

  ExcludeLite&
  getExclude() { return ExcludeLite::downCast(exclude); }

  const ExcludeLite&
  getExclude() const { return ExcludeLite::downCast(exclude); }

  int
  getChildSelector() const { return childSelector; }

  /**
   * Return true if the content must be fresh. The default is true.
   * @return true if must be fresh, otherwise false.
   */
  bool
  getMustBeFresh() const;

  Milliseconds
  getInterestLifetimeMilliseconds() const { return interestLifetimeMilliseconds; }

  const BlobLite&
  getNonce() const { return BlobLite::downCast(nonce); }

  /**
   * Get the forwarding hint, encoded as a sequence of delegations without the
   * outer TLV type and length (which has already been removed).
   * @return The forwarding hint encoding.
   */
  const BlobLite&
  getForwardingHintWireEncoding() const
  {
    return BlobLite::downCast(forwardingHintWireEncoding);
  }

  const BlobLite&
  getLinkWireEncoding() const { return BlobLite::downCast(linkWireEncoding); }

  int
  getSelectedDelegationIndex() const { return selectedDelegationIndex; }

  /**
   * Set this interest's name to have the values from the given name.
   * @param name The name to get values from.
   * @return 0 for success, or an error code if there is not enough room in this
   * object's name components array.
   */
  ndn_Error
  setName(const NameLite& name)
  {
    return NameLite::downCast(this->name).set(name);
  }

  /**
   * Set the min suffix components count.
   * @param minSuffixComponents The min suffix components count. If not
   * specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setMinSuffixComponents(int minSuffixComponents)
  {
    this->minSuffixComponents = minSuffixComponents;
    return *this;
  }

  /**
   * Set the max suffix components count.
   * @param maxSuffixComponents The max suffix components count. If not
   * specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setMaxSuffixComponents(int maxSuffixComponents)
  {
    this->maxSuffixComponents = maxSuffixComponents;
    return *this;
  }

  /**
   * Set the child selector.
   * @param childSelector The child selector. If not specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setChildSelector(int childSelector)
  {
    this->childSelector = childSelector;
    return *this;
  }

  /**
   * Set the MustBeFresh flag.
   * @param mustBeFresh True if the content must be fresh, otherwise false. If
   * you do not set this flag, the default value is true.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setMustBeFresh(bool mustBeFresh);

  /**
   * Set the interest lifetime.
   * @param interestLifetimeMilliseconds The interest lifetime in milliseconds.
   * If not specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setInterestLifetimeMilliseconds(Milliseconds interestLifetimeMilliseconds)
  {
    this->interestLifetimeMilliseconds = interestLifetimeMilliseconds;
    return *this;
  }

  /**
   * Set the interest nonce.
   * @param nonce The nonce value. This does not copy the bytes of the nonce.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setNonce(const BlobLite& nonce)
  {
    BlobLite::downCast(this->nonce) = nonce;
    return *this;
  }

  /**
   * Set the forwarding hint, encoded as a sequence of delegations without the
   * outer TLV type and length (which will be added later).
   * @param nonce The encoding value. This does not copy the bytes of the
   * encoding.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setForwardingHintWireEncoding(const BlobLite& forwardingHintWireEncoding)
  {
    BlobLite::downCast(this->forwardingHintWireEncoding) =
      forwardingHintWireEncoding;
    return *this;
  }

  /**
   * Set the link wire encoding.
   * @param linkWireEncoding The encoding value. This does not copy the bytes of
   * the encoding.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setLinkWireEncoding(const BlobLite& linkWireEncoding)
  {
    BlobLite::downCast(this->linkWireEncoding) = linkWireEncoding;
    return *this;
  }

  /**
   * Set the selected delegation index.
   * @param selectedDelegationIndex The selected delegation index. If not
   * specified, set to -1.
   * @return This InterestLite so that you can chain calls to update values.
   */
  InterestLite&
  setSelectedDelegationIndex(int selectedDelegationIndex)
  {
    this->selectedDelegationIndex = selectedDelegationIndex;
    return *this;
  }

  /**
   * Set this interest object to have the values from the other interest.
   * @param other The other InterestLite to get values from.
   * @return 0 for success, or an error code if there is not enough room in this
   * object's name components array or exclude entries array.
   */
  ndn_Error
  set(const InterestLite& other);

  /**
   * Downcast the reference to the ndn_Interest struct to a InterestLite.
   * @param interest A reference to the ndn_Interest struct.
   * @return The same reference as InterestLite.
   */
  static InterestLite&
  downCast(ndn_Interest& interest) { return *(InterestLite*)&interest; }

  static const InterestLite&
  downCast(const ndn_Interest& interest) { return *(InterestLite*)&interest; }

private:
  // Declare friends who can downcast to the private base.
  friend class Tlv0_2WireFormatLite;

  /**
   * Don't allow the copy constructor. Instead use set(const InterestLite&)
   * which can return an error if there is no more room in the name components
   * array.
   */
  InterestLite(const InterestLite& other);

  /**
   * Don't allow the assignment operator. Instead use set(const InterestLite&)
   * which can return an error if there is no more room in the name components
   * array.
   */
  InterestLite& operator=(const InterestLite& other);
};

}

#endif
