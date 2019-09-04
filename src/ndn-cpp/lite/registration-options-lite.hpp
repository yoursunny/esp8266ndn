/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2019 Regents of the University of California.
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

#ifndef NDN_REGISTRATION_OPTIONS_LITE_HPP
#define NDN_REGISTRATION_OPTIONS_LITE_HPP

#include "../c/registration-options-types.h"

namespace ndn {

/**
 * A RegistrationOptionsLite holds the options used when registering with the
 * forwarder to specify how to forward an interest and other options. We use a
 * separate RegistrationOptionsLite object to retain future compatibility if the
 * format of the registration command is changed.
 * (This class was renamed from ForwardingFlagsLite, which is deprecated.)
 */
class RegistrationOptionsLite : protected ndn_RegistrationOptions {
public:
  /**
   * Create a RegistrationOptionsLite with "childInherit" set and all other flags
   * cleared.
   */
  RegistrationOptionsLite();

  /**
   * Get the value of the "childInherit" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool
  getChildInherit() const { return childInherit != 0; }

  /**
   * Get the value of the "capture" flag.
   * @return true if the flag is set, false if it is cleared.
   */
  bool
  getCapture() const { return capture != 0; }

  int
  getOrigin() const { return origin; }

  /**
   * Set the value of the "childInherit" flag
   * @param childInherit true to set the flag, false to clear it.
   * @return This RegistrationOptionsLite so that you can chain calls to update values.
   */
  RegistrationOptionsLite&
  setChildInherit(bool childInherit)
  {
    this->childInherit = childInherit ? 1 : 0;
    return *this;
  }

  /**
   * Set the value of the "capture" flag
   * @param capture true to set the flag, false to clear it.
   * @return This RegistrationOptionsLite so that you can chain calls to update values.
   */
  RegistrationOptionsLite&
  setCapture(bool capture)
  {
    this->capture = capture ? 1 : 0;
    return *this;
  }

  /**
   * Set the origin value.
   * @param origin The new origin value, or -1 for not specified.
   * @return This RegistrationOptionsLite so that you can chain calls to update
   * values.
   */
  RegistrationOptionsLite&
  setOrigin(int origin)
  {
    this->origin = origin;
    return *this;
  }

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
   * This ignores the origin value.
   * @param nfdForwardingFlags An integer with the bits set.
   * @return This RegistrationOptionsLite so that you can chain calls to update values.
   */
  RegistrationOptionsLite&
  setNfdForwardingFlags(int nfdForwardingFlags);

  /**
   * Downcast the reference to the ndn_RegistrationOptions struct to a
   * RegistrationOptionsLite.
   * @param registrationOptions A reference to the ndn_RegistrationOptions struct.
   * @return The same reference as RegistrationOptionsLite.
   */
  static RegistrationOptionsLite&
  downCast(ndn_RegistrationOptions& registrationOptions)
  {
    return *(RegistrationOptionsLite*)&registrationOptions;
  }

  static const RegistrationOptionsLite&
  downCast(const ndn_RegistrationOptions& registrationOptions)
  {
    return *(RegistrationOptionsLite*)&registrationOptions;
  }
};


}

#endif
