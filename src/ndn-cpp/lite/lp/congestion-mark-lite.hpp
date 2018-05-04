/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2018 Regents of the University of California.
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

#ifndef NDN_CONGESTION_MARK_LITE_HPP
#define NDN_CONGESTION_MARK_LITE_HPP

#include "../../c/lp/congestion-mark-types.h"

namespace ndn {

class LpPacketLite;

/**
 * CongestionMarkLite represents the congestion mark header field in an NDNLPv2
 * packet.
 * http://redmine.named-data.net/projects/nfd/wiki/NDNLPv2
 */
class CongestionMarkLite : private ndn_CongestionMark {
public:
  /**
   * Create a CongestionMarkLite where all the values are unspecified.
   */
  CongestionMarkLite();

  /**
   * Get the congestion mark value.
   * @return The congestion mark value.
   */
  uint64_t
  getCongestionMark() const { return congestionMark; }

  /**
   * Set the congestion mark value.
   * @param congestionMark The congestion mark value.
   */
  void
  setCongestionMark(uint64_t congestionMark) { this->congestionMark = congestionMark; }

  /**
   * Get the first header field in lpPacket which is a CongestionMarkLite.
   * @param lpPacket The ndn_LpPacket struct with the header fields to search.
   * @return A pointer to the first CongestionMarkLite header field, or 0 if not
   * found.
   */
  static const CongestionMarkLite*
  getFirstHeader(const LpPacketLite& lpPacket);

  /**
   * Downcast the reference to the ndn_CongestionMark struct to an
   * CongestionMarkLite.
   * @param congestionMark A reference to the ndn_CongestionMark struct.
   * @return The same reference as CongestionMarkLite.
   */
  static CongestionMarkLite&
  downCast(ndn_CongestionMark& congestionMark)
  {
    return *(CongestionMarkLite*)&congestionMark;
  }

  static const CongestionMarkLite&
  downCast(const ndn_CongestionMark& congestionMark)
  {
    return *(CongestionMarkLite*)&congestionMark;
  }
};

}

#endif
