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

#ifndef NDN_VALIDITY_PERIOD_LITE_HPP
#define NDN_VALIDITY_PERIOD_LITE_HPP

#include "../../c/security/validity-period-types.h"

namespace ndn {

/**
 * A ValidityPeriodLite is used in a Data packet's SignatureInfo and represents
 * the begin and end times of a certificate's validity period.
 */
class ValidityPeriodLite : private ndn_ValidityPeriod {
public:
  /**
   * Create a default ValidityPeriodLite where the period is not specified.
   */
  ValidityPeriodLite();

  /**
   * Check if the period has been set.
   * @return True if the period has been set, false if the period is not
   * specified (after calling the default constructor or clear).
   */
  bool
  hasPeriod() const;

  /**
   * Get the beginning of the validity period range.
   * @return The time as milliseconds since Jan 1, 1970 UTC.
   */
  ndn_MillisecondsSince1970
  getNotBefore() const { return notBefore; }

  /**
   * Get the end of the validity period range.
   * @return The time as milliseconds since Jan 1, 1970 UTC.
   */
  ndn_MillisecondsSince1970
  getNotAfter() const { return notAfter; }

  /** Reset to a default ValidityPeriod where the period is not specified.
   */
  void
  clear();

  /**
   * Set the validity period.
   * @param notBefore The beginning of the validity period range as milliseconds
   * since Jan 1, 1970 UTC. Note that this is rounded up to the nearest whole
   * second.
   * @param notAfter The end of the validity period range as milliseconds
   * since Jan 1, 1970 UTC. Note that this is rounded down to the nearest whole
   * second.
   * @return This ValidityPeriodLite so that you can chain calls to update values.
   */
  ValidityPeriodLite&
  setPeriod
    (ndn_MillisecondsSince1970 notBefore, ndn_MillisecondsSince1970 notAfter);

  /**
   * Check if this is the same validity period as other.
   * @param other The other ValidityPeriodLite to compare with.
   * @return True if the validity periods are equal.
   */
  bool
  equals(const ValidityPeriodLite& other) const;

  /**
   * Check if the time falls within the validity period.
   * @param time The time to check as milliseconds since Jan 1, 1970 UTC.
   * @return True if the beginning of the validity period is less than or equal
   * to time and time is less than or equal to the end of the validity period.
   */
  bool
  isValid(ndn_MillisecondsSince1970 time) const;

  /**
   * Downcast the reference to the ndn_ValidityPeriod struct to a ValidityPeriodLite.
   * @param validityPeriod A reference to the ndn_ValidityPeriod struct.
   * @return The same reference as ValidityPeriodLite.
   */
  static ValidityPeriodLite&
  downCast(ndn_ValidityPeriod& validityPeriod) { return *(ValidityPeriodLite*)&validityPeriod; }

  static const ValidityPeriodLite&
  downCast(const ndn_ValidityPeriod& validityPeriod) { return *(ValidityPeriodLite*)&validityPeriod; }
};

}

#endif
