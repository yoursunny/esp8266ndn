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

#ifndef NDN_COMMON_H
#define NDN_COMMON_H

#include "../ndn-cpp-config.h"
#include <stdint.h>
#include <stddef.h>

#if NDN_CPP_HAVE_ATTRIBUTE_DEPRECATED
  #define DEPRECATED_IN_NDN_CPP __attribute__((deprecated))
#else
  #define DEPRECATED_IN_NDN_CPP
#endif

#if !NDN_CPP_HAVE_ROUND
#define round(x) floor((x) + 0.5)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A time interval represented as the number of milliseconds.
 */
typedef double ndn_Milliseconds;

/**
 * The calendar time represented as the number of milliseconds since 1/1/1970.
 */
typedef double ndn_MillisecondsSince1970;

/**
 * Get the current time in milliseconds. (For efficiency, this does not convert
 * to UTC time.)
 * @return The current time in milliseconds since 1/1/1970, including fractions
 * of a millisecond (according to timeval.tv_usec).
 */
ndn_MillisecondsSince1970
ndn_getNowMilliseconds();

/**
 * The practical limit of the size of a network-layer packet. If a packet is
 * larger than this, the library or application MAY drop it. This constant is
 * defined in this low-level header file so that internal code can use it, but
 * applications should use the static inline API method
 * Face::getMaxNdnPacketSize() which is equivalent.
 */
static const size_t MAX_NDN_PACKET_SIZE = 8800;

/**
 * The size in bytes of a SHA-256 digest. We define this separately so that we
 * don't have to include the openssl header everywhere.
 */
static const size_t ndn_SHA256_DIGEST_SIZE = 32;

#ifdef __cplusplus
}
#endif

#endif
