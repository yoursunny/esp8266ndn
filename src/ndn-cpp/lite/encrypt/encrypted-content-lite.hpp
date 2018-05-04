/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2018 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-group-encrypt src/encrypted-content https://github.com/named-data/ndn-group-encrypt
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

#ifndef NDN_ENCRYPTED_CONTENT_LITE_HPP
#define NDN_ENCRYPTED_CONTENT_LITE_HPP

#include "../key-locator-lite.hpp"
#include "../../c/encrypt/encrypted-content-types.h"

namespace ndn {

/**
 * An EncryptedContentLite holds an encryption type, a payload and other fields
 * representing encrypted content.
 * @note This class is an experimental feature. The API may change.
 */
class EncryptedContentLite : private ndn_EncryptedContent {
public:
  /**
   * Create a EncryptedContentLite with values for none.
   * @param keyNameComponents The pre-allocated array of ndn_NameComponent for
   * the keyLocatorLite. Instead of an array of NameLite::Component, this is an
   * array of the underlying ndn_NameComponent struct so that it doesn't run the
   * default constructor unnecessarily.
   * @param maxKeyNameComponents The number of elements in the allocated
   * keyNameComponents array.
   */
  EncryptedContentLite
    (ndn_NameComponent* keyNameComponents, size_t maxKeyNameComponents);

  /**
   * Get the algorithm type.
   * @return The algorithm type, or -1 if not specified.
   */
  ndn_EncryptAlgorithmType
  getAlgorithmType() const { return algorithmType; }

  /**
   * Get the key locator.
   * @return The key locator. If not specified, getType() is -1.
   */
  const KeyLocatorLite&
  getKeyLocator() const { return KeyLocatorLite::downCast(keyLocator); }

  KeyLocatorLite&
  getKeyLocator() { return KeyLocatorLite::downCast(keyLocator); }

  /**
   * Get the initial vector.
   * @return The initial vector. If not specified, isNull() is true.
   */
  const BlobLite&
  getInitialVector() const { return BlobLite::downCast(initialVector); }

  /**
   * Get the payload.
   * @return The payload. If not specified, isNull() is true.
   */
  const BlobLite&
  getPayload() const { return BlobLite::downCast(payload); }

  /**
   * Set the algorithm type.
   * @param algorithmType The algorithm type. If not specified, set to -1.
   * @return This EncryptedContentLite so that you can chain calls to update
   * values.
   */
  EncryptedContentLite&
  setAlgorithmType(ndn_EncryptAlgorithmType algorithmType)
  {
    this->algorithmType = algorithmType;
    return *this;
  }

  /**
   * Set the initial vector.
   * @param initialVector The initial vector. If not specified, set to the
   * default BlobLite() where isNull() is true.
   * @return This EncryptedContentLite so that you can chain calls to update
   * values.
   */
  EncryptedContentLite&
  setInitialVector(const BlobLite& initialVector)
  {
    BlobLite::downCast(this->initialVector) = initialVector;
    return *this;
  }

  /**
   * Set the encrypted payload.
   * @param payload The encrypted payload. If not specified, set to the
   * default BlobLite() where isNull() is true.
   * @return This EncryptedContentLite so that you can chain calls to update
   * values.
   */
  EncryptedContentLite&
  setPayload(const BlobLite& payload)
  {
    BlobLite::downCast(this->payload) = payload;
    return *this;
  }

  /**
   * Set this encryptedContent to have the values from the other encryptedContent.
   * @param other The other EncryptedContentLite to get values from.
   * @return 0 for success, or an error code if there is not enough room in this
   * object's key locator keyName components array.
   */
  ndn_Error
  set(const EncryptedContentLite& other);

  /**
   * Downcast the reference to the ndn_EncryptedContent struct to an
   * EncryptedContentLite.
   * @param encryptedContent A reference to the ndn_EncryptedContent struct.
   * @return The same reference as EncryptedContentLite.
   */
  static EncryptedContentLite&
  downCast(ndn_EncryptedContent& encryptedContent)
  {
    return *(EncryptedContentLite*)&encryptedContent;
  }

  static const EncryptedContentLite&
  downCast(const ndn_EncryptedContent& encryptedContent)
  {
    return *(EncryptedContentLite*)&encryptedContent;
  }

private:
  // Declare friends who can downcast to the private base.
  friend class Tlv0_2WireFormatLite;

  /**
   * Don't allow the copy constructor. Instead use
   * set(const EncryptedContentLite&) which can return an error if there is no
   * more room in the name components array.
   */
  EncryptedContentLite(const EncryptedContentLite& other);

  /**
   * Don't allow the assignment operator. Instead use
   * set(const EncryptedContentLite&) which can return an error if there is no
   * more room in the name components array.
   */
  EncryptedContentLite& operator=(const EncryptedContentLite& other);
};

}

#endif
