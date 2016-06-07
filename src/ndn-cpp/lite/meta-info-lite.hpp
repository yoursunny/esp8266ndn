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

  ndn_ContentType
  getType() const { return type; }

  Milliseconds
  getFreshnessPeriod() const { return freshnessPeriod; }

  const NameLite::Component
  getFinalBlockId() const { return NameLite::Component::downCast(finalBlockId); }

  void
  setType(ndn_ContentType type) { this->type = type; }

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
