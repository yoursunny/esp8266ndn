/**
 * Copyright (C) 2018-2019 Regents of the University of California.
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

#ifndef NDN_CONGESTION_MARK_H
#define NDN_CONGESTION_MARK_H

#include "lp-packet.h"
#include "../../c/lp/congestion-mark-types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_LpPacket;

/**
 * Initialize an ndn_CongestionMark struct where all the values are unspecified.
 * @param self A pointer to the ndn_CongestionMark struct.
 */
static __inline void
ndn_CongestionMark_initialize(struct ndn_CongestionMark *self)
{
  self->congestionMark = 0;
}

/**
 * Get the first header field in lpPacket which is an ndn_CongestionMark.
 * @param lpPacket The ndn_LpPacket struct with the header fields to search.
 * @return A pointer to the first ndn_CongestionMark header field, or 0 if not
 * found.
 */
const struct ndn_CongestionMark *
ndn_CongestionMark_getFirstHeader(const struct ndn_LpPacket *lpPacket);

/**
 * Add an ndn_CongestionMark header field to lpPacket and initialize it.
 * @param lpPacket A pointer to the ndn_LpPacket struct to update.
 * @param congestionMark Set congestionMark to a pointer to the newly-added
 * ndn_CongestionMark struct.
 * @return 0 for success, or an error code if there is no more room in the
 * fieldHeaders array (nHeaderFields is already maxHeaderFields).
 */
ndn_Error
ndn_CongestionMark_add
  (struct ndn_LpPacket *lpPacket, struct ndn_CongestionMark **congestionMark);

#ifdef __cplusplus
}
#endif

#endif
