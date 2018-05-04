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

#ifndef NDN_META_INFO_LITE_HPP
#define NDN_META_INFO_LITE_HPP

#include "common-lite.hpp"
#include "name-lite.hpp"
#include "../c/data-types.h"

namespace ndn {

/**
 * A MetaInfoLite holds a type and other info to represent the meta info of a
 * Data packet.
 */
class MetaInfoLite : private ndn_MetaInfo {
public:
  /**
   * Create a MetaInfoLite with default field values.
   */
  MetaInfoLite();

  /**
   * Get the content type.
   * @return The content type enum value. If this is ndn_ContentType_OTHER_CODE,
   * then call getOtherTypeCode() to get the unrecognized content type code.
   */
  ndn_ContentType
  getType() const { return type; }

  /**
   * Get the content type code from the packet which is other than a recognized
   * ContentType enum value. This is only meaningful if getType() is
   * ndn_ContentType_OTHER_CODE.
   * @return The type code.
   */
  int
  getOtherTypeCode() const { return otherTypeCode; }

  Milliseconds
  getFreshnessPeriod() const { return freshnessPeriod; }

  const NameLite::Component
  getFinalBlockId() const { return NameLite::Component::downCast(finalBlockId); }

  /**
   * Set the content type.
   * @param type The content type enum value. If the packet's content type is
   * not a recognized ContentType enum value, use ndn_ContentType_OTHER_CODE and
   * call setOtherTypeCode().
   */
  void
  setType(ndn_ContentType type) { this->type = type; }

  /**
   * Set the packet's content type code to use when the content type enum is
   * ndn_ContentType_OTHER_CODE. If the packet's content type code is a
   * recognized enum value, just call setType().
   * @param otherTypeCode The packet's unrecognized content type code, which
   * must be non-negative.
   */
  void
  setOtherTypeCode(int otherTypeCode) { this->otherTypeCode = otherTypeCode; }

  void
  setFreshnessPeriod(Milliseconds freshnessPeriod)
  {
    this->freshnessPeriod = freshnessPeriod;
  }

  /**
   * Set the final block ID.
   * @param finalBlockId The final block ID. This takes the pointer but does not
   * copy the bytes of the name component.
   */
  void
  setFinalBlockId(const NameLite::Component& finalBlockId)
  {
    NameLite::Component::downCast(this->finalBlockId) = finalBlockId;
  }

  /**
   * Downcast the reference to the ndn_MetaInfo struct to a MetaInfoLite.
   * @param metaInfo A reference to the ndn_MetaInfo struct.
   * @return The same reference as MetaInfoLite.
   */
  static MetaInfoLite&
  downCast(ndn_MetaInfo& metaInfo) { return *(MetaInfoLite*)&metaInfo; }

  static const MetaInfoLite&
  downCast(const ndn_MetaInfo& metaInfo) { return *(MetaInfoLite*)&metaInfo; }
};

}

#endif
