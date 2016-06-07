/**
 * Copyright (C) 2016 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-cxx nack.hpp https://github.com/named-data/ndn-cxx/blob/master/src/lp/nack.hpp
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

#ifndef NDN_NETWORK_NACK_H
#define NDN_NETWORK_NACK_H

#include "lp/lp-packet.h"
#include <ndn-cpp/c/network-nack-types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_LpPacket;

/**
 * Initialize an ndn_NetworkNack struct where all the values are unspecified.
 * @param self A pointer to the ndn_NetworkNack struct.
 */
static __inline void
ndn_NetworkNack_initialize(struct ndn_NetworkNack *self)
{
  self->reason = ndn_NetworkNackReason_NONE;
  self->otherReasonCode = -1;
}

/**
 * Get the first header field in lpPacket which is an ndn_NetworkNack.
 * @param lpPacket The ndn_LpPacket struct with the header fields to search.
 * @return A pointer to the first ndn_NetworkNack header field, or 0 if not
 * found.
 */
const struct ndn_NetworkNack *
ndn_NetworkNack_getFirstHeader(const struct ndn_LpPacket *lpPacket);

/**
 * Add an ndn_NetworkNack header field to lpPacket and initialize it.
 * @param lpPacket A pointer to the ndn_LpPacket struct to update.
 * @param networkNack Set networkNack to a pointer to the newly-added
 * ndn_NetworkNack struct.
 * @return 0 for success, or an error code if there is no more room in the
 * fieldHeaders array (nHeaderFields is already maxHeaderFields).
 */
ndn_Error
ndn_NetworkNack_add
  (struct ndn_LpPacket *lpPacket, struct ndn_NetworkNack **networkNack);

#ifdef __cplusplus
}
#endif

#endif
