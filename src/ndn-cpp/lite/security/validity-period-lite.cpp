/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
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

#include "../../c/security/validity-period.h"
#include "../../lite/security/validity-period-lite.hpp"

namespace ndn {

ValidityPeriodLite::ValidityPeriodLite()
{
  ndn_ValidityPeriod_initialize(this);
}

bool
ValidityPeriodLite::hasPeriod() const
{
  return ndn_ValidityPeriod_hasPeriod(this) != 0;
}

void
ValidityPeriodLite::clear()
{
  ndn_ValidityPeriod_clear(this);
}

ValidityPeriodLite&
ValidityPeriodLite::setPeriod
  (ndn_MillisecondsSince1970 notBefore, ndn_MillisecondsSince1970 notAfter)
{
  ndn_ValidityPeriod_setPeriod(this, notBefore, notAfter);
  return *this;
}

bool
ValidityPeriodLite::equals(const ValidityPeriodLite& other) const
{
  return ndn_ValidityPeriod_equals(this, &other) != 0;
}

bool
ValidityPeriodLite::isValid(ndn_MillisecondsSince1970 time) const
{
  return ndn_ValidityPeriod_isValid(this, time) != 0;
}

}
