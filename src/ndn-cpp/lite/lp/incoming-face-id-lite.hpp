/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2018 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-cxx fields.hpp https://github.com/named-data/ndn-cxx/blob/master/src/lp/fields.hpp
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

#ifndef NDN_INCOMING_FACE_ID_LITE_HPP
#define NDN_INCOMING_FACE_ID_LITE_HPP

#include "../../c/lp/incoming-face-id-types.h"

namespace ndn {

class LpPacketLite;

/**
 * IncomingFaceIdLite represents the incoming face ID header field in an NDNLPv2
 * packet.
 * http://redmine.named-data.net/projects/nfd/wiki/NDNLPv2
 */
class IncomingFaceIdLite : private ndn_IncomingFaceId {
public:
  /**
   * Create a IncomingFaceIdLite where all the values are unspecified.
   */
  IncomingFaceIdLite();

  /**
   * Get the incoming face ID value.
   * @return The face ID value.
   */
  uint64_t
  getFaceId() const { return faceId; }

  /**
   * Set the face ID value.
   * @param faceId The incoming face ID value.
   */
  void
  setFaceId(uint64_t faceId) { this->faceId = faceId; }

  /**
   * Get the first header field in lpPacket which is an IncomingFaceIdLite.
   * @param lpPacket The ndn_LpPacket struct with the header fields to search.
   * @return A pointer to the first IncomingFaceIdLite header field, or 0 if not
   * found.
   */
  static const IncomingFaceIdLite*
  getFirstHeader(const LpPacketLite& lpPacket);

  /**
   * Downcast the reference to the ndn_IncomingFaceId struct to an
   * IncomingFaceIdLite.
   * @param incomingFaceId A reference to the ndn_IncomingFaceId struct.
   * @return The same reference as IncomingFaceIdLite.
   */
  static IncomingFaceIdLite&
  downCast(ndn_IncomingFaceId& incomingFaceId)
  {
    return *(IncomingFaceIdLite*)&incomingFaceId;
  }

  static const IncomingFaceIdLite&
  downCast(const ndn_IncomingFaceId& incomingFaceId)
  {
    return *(IncomingFaceIdLite*)&incomingFaceId;
  }
};

}

#endif
