/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#ifndef NDN_VALIDITY_PERIOD_TYPES_H
#define NDN_VALIDITY_PERIOD_TYPES_H

#include "../common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_ValidityPeriod is used in a Data packet's SignatureInfo and represents
 * the begin and end times of a certificate's validity period.
 */
struct ndn_ValidityPeriod {
  ndn_MillisecondsSince1970 notBefore; /**< DBL_MAX for none. */
  ndn_MillisecondsSince1970 notAfter; /**< -DBL_MAX for none. */
};

#ifdef __cplusplus
}
#endif

#endif
