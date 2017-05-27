/**
 * Copyright (C) 2016-2017 Regents of the University of California.
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

#ifndef NDN_VALIDITY_PERIOD_H
#define NDN_VALIDITY_PERIOD_H

#include <float.h>
#include <math.h>
#include "../../c/security/validity-period-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reset to a default ValidityPeriod where the period is not specified.
 * @param self A pointer to the ndn_ValidityPeriod struct.
 */
static __inline void
ndn_ValidityPeriod_clear(struct ndn_ValidityPeriod *self)
{
  self->notBefore = DBL_MAX;
  self->notAfter = -DBL_MAX;
}

/**
 * Initialize a default ValidityPeriod where the period is not specified.
 * @param self A pointer to the ndn_ValidityPeriod struct.
 */
static __inline void
ndn_ValidityPeriod_initialize(struct ndn_ValidityPeriod *self)
{
  ndn_ValidityPeriod_clear(self);
}

/**
 * Check if the period has been set.
 * @param self A pointer to the ndn_ValidityPeriod struct.
 * @return Non-zero if the period has been set, zero if the period is not
 * specified (after calling the default constructor or clear).
 */
static __inline int
ndn_ValidityPeriod_hasPeriod(const struct ndn_ValidityPeriod *self)
{
  return !(self->notBefore == DBL_MAX && self->notAfter == -DBL_MAX);
}

/**
 * Set the validity period.
 * @param self A pointer to the ndn_ValidityPeriod struct.
 * @param notBefore The beginning of the validity period range as milliseconds
 * since Jan 1, 1970 UTC. Note that this is rounded up to the nearest whole
 * second.
 * @param notAfter The end of the validity period range as milliseconds
 * since Jan 1, 1970 UTC. Note that this is rounded down to the nearest whole
 * second.
 */
static __inline void
ndn_ValidityPeriod_setPeriod
  (struct ndn_ValidityPeriod *self, ndn_MillisecondsSince1970 notBefore,
   ndn_MillisecondsSince1970 notAfter)
{
  // Round up to the nearest second.
  self->notBefore = round(ceil(round(notBefore) / 1000.0) * 1000.0);
  // Round down to the nearest second.
  self->notAfter = round(floor(round(notAfter) / 1000.0) * 1000.0);
}

/**
 * Check if this is the same validity period as other.
 * @param other The other ValidityPeriodLite to compare with.
 * @return Non-zero if the validity periods are equal, otherwise zerp.
 */
static __inline int
ndn_ValidityPeriod_equals
  (const struct ndn_ValidityPeriod *self, const struct ndn_ValidityPeriod *other)
{
  return self->notBefore == other->notBefore &&
         self->notAfter == other->notAfter;
}

/**
 * Check if the time falls within the validity period.
 * @param self A pointer to the ndn_ValidityPeriod struct.
 * @param time The time to check as milliseconds since Jan 1, 1970 UTC.
 * @return True if the beginning of the validity period is less than or equal
 * to time and time is less than or equal to the end of the validity period.
 */
static __inline int
ndn_ValidityPeriod_isValid
  (const struct ndn_ValidityPeriod *self, ndn_MillisecondsSince1970 time)
{
  return self->notBefore <= time && time <= self->notAfter;
}

#ifdef __cplusplus
}
#endif

#endif
