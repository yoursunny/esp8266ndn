/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2019 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-cxx nack.hpp https://github.com/named-data/ndn-cxx/blob/master/ndn-cxx/lp/nack.hpp
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

#ifndef NDN_NETWORK_NACK_LITE_HPP
#define NDN_NETWORK_NACK_LITE_HPP

#include "../c/network-nack-types.h"

namespace ndn {

class LpPacketLite;

/**
 * NetworkNackLite represents a network Nack packet and includes a Nack reason.
 */
class NetworkNackLite : private ndn_NetworkNack {
public:
  /**
   * Create a NetworkNackLite where all the values are unspecified.
   */
  NetworkNackLite();

  /**
   * Get the network Nack reason.
   * @return The reason enum value. If this is ndn_NetworkNackReason_OTHER_CODE,
   * then call getOtherReasonCode() to get the unrecognized reason code.
   */
  ndn_NetworkNackReason
  getReason() const { return reason; }

  /**
   * Get the reason code from the packet which is other than a recognized
   * ndn_NetworkNackReason enum value. This is only meaningful if getReason() is
   * ndn_NetworkNackReason_OTHER_CODE.
   * @return The reason code.
   */
  int
  getOtherReasonCode() const { return otherReasonCode; }

  /**
   * Set the network Nack reason.
   * @param reason The network Nack reason enum value. If the packet's reason
   * code is not a recognized Reason enum value, use
   * ndn_NetworkNackReason_OTHER_CODE and call setOtherReasonCode().
   */
  void
  setReason(ndn_NetworkNackReason reason) { this->reason = reason; }

  /**
   * Set the packet's reason code to use when the reason enum is
   * ndn_NetworkNackReason_OTHER_CODE. If the packet's reason code is a
   * recognized enum value, just call setReason().
   * @param otherReasonCode The packet's unrecognized reason code, which must be
   * non-negative.
   */
  void
  setOtherReasonCode(int otherReasonCode)
  {
    this->otherReasonCode = otherReasonCode;
  }

  /**
   * Get the first header field in lpPacket which is a NetworkNackLite.
   * @param lpPacket The ndn_LpPacket struct with the header fields to search.
   * @return A pointer to the first NetworkNackLite header field, or 0 if not
   * found.
   */
  static const NetworkNackLite*
  getFirstHeader(const LpPacketLite& lpPacket);

  /**
   * Downcast the reference to the ndn_NetworkNack struct to an
   * NetworkNackLite.
   * @param networkNack A reference to the ndn_NetworkNack struct.
   * @return The same reference as NetworkNackLite.
   */
  static NetworkNackLite&
  downCast(ndn_NetworkNack& networkNack)
  {
    return *(NetworkNackLite*)&networkNack;
  }

  static const NetworkNackLite&
  downCast(const ndn_NetworkNack& networkNack)
  {
    return *(NetworkNackLite*)&networkNack;
  }
};

}

#endif
