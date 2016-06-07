/**
 * Copyright (C) 2013-2016 Regents of the University of California.
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

#ifndef NDN_TIME_H
#define NDN_TIME_H

#include <ndn-cpp/c/common.h>
#include <ndn-cpp/c/errors.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * Convert the time from milliseconds to an ISO time string, for example "20131018T184138.423355".
 * @param milliseconds The time in milliseconds since 1/1/1970, including fractions of a millisecond.
 * @param isoString A buffer of at least 23 bytes to receive the null-terminated ISO time string.
 * @return 0 for success, else an error code including if we don't have necessary standard library support.
 */
ndn_Error
ndn_toIsoString(ndn_MillisecondsSince1970 milliseconds, char *isoString);

/**
 * Parse the ISO time string and return the time in milliseconds.
 * @param isoString The ISO time string, for example "20131018T184138.423355".
 * @param milliseconds Return the time in milliseconds since 1/1/1970, including fractions of a millisecond.
 * @return 0 for success, else an error code including if we don't have necessary standard library support.
 */
ndn_Error
ndn_fromIsoString(const char* isoString, ndn_MillisecondsSince1970 *milliseconds);

#ifdef  __cplusplus
}
#endif

#endif
