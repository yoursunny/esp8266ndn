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

#ifndef NDN_FORWARDING_FLAGS_LITE_HPP
#define NDN_FORWARDING_FLAGS_LITE_HPP

#include "../c/forwarding-flags.h"

namespace ndn {

/**
 * A ForwardingFlagsLite holds the flags which specify how the forwarding daemon
 * should forward an interest for a registered prefix.  We use a separate
 * ForwardingFlagsLite object to retain future compatibility if the daemon
 * forwarding bits are changed, amended or deprecated.
 */
class ForwardingFlagsLite : private ndn_ForwardingFlags {
public:
  /**
   * Create a ForwardingFlagsLite with "childInherit" set and all other flags
   * cleared.
   */
  ForwardingFlagsLite();

  /**
   * Get the value of the "childInherit" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getChildInherit() const { return childInherit != 0; }

  /**
   * Get the value of the "capture" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool getCapture() const { return capture != 0; }

  /**
   * Set the value of the "childInherit" flag
   * @param childInherit true to set the flag, false to clear it.
   */
  void setChildInherit(bool childInherit) { this->childInherit = childInherit ? 1 : 0; }

  /**
   * Set the value of the "capture" flag
   * @param capture true to set the flag, false to clear it.
   */
  void setCapture(bool capture) { this->capture = capture ? 1 : 0; }

  /**
   * Get an integer with the bits set according to the NFD forwarding flags as
   * used in the ControlParameters of the command interest.
   * @return An integer with the bits set.
   */
  int
  getNfdForwardingFlags() const;

  /**
   * Set the flags according to the NFD forwarding flags as used in the
   * ControlParameters of the command interest.
   * @param nfdForwardingFlags An integer with the bits set.
   */
  void
  setNfdForwardingFlags(int nfdForwardingFlags);

  /**
   * Downcast the reference to the ndn_ForwardingFlags struct to a
   * ForwardingFlagsLite.
   * @param forwardingFlags A reference to the ndn_ForwardingFlags struct.
   * @return The same reference as ForwardingFlagsLite.
   */
  static ForwardingFlagsLite&
  downCast(ndn_ForwardingFlags& forwardingFlags)
  {
    return *(ForwardingFlagsLite*)&forwardingFlags;
  }

  static const ForwardingFlagsLite&
  downCast(const ndn_ForwardingFlags& forwardingFlags)
  {
    return *(ForwardingFlagsLite*)&forwardingFlags;
  }
};

}

#endif
