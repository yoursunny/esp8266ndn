/**
 * Copyright (C) 2016 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-cxx fields.hpp https://github.com/named-data/ndn-cxx/blob/master/src/lp/fields.hpp
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

/* Note: This file should be named forward-flags.h (like data.h) but
 * we make it forwarding-flags-impl.h because forwarding-flags.h is already in
 * the public include directory for backwards compatibility reasons.
 */

#ifndef NDN_INCOMING_FACE_ID_H
#define NDN_INCOMING_FACE_ID_H

#include "lp-packet.h"
#include <ndn-cpp/c/lp/incoming-face-id-types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_LpPacket;

/**
 * Initialize an ndn_IncomingFaceId struct where all the values are unspecified.
 * @param self A pointer to the ndn_IncomingFaceId struct.
 */
static __inline void
ndn_IncomingFaceId_initialize(struct ndn_IncomingFaceId *self)
{
  self->faceId = (uint64_t)-1;
}

/**
 * Get the first header field in lpPacket which is an ndn_IncomingFaceId.
 * @param lpPacket The ndn_LpPacket struct with the header fields to search.
 * @return A pointer to the first ndn_IncomingFaceId header field, or 0 if not
 * found.
 */
const struct ndn_IncomingFaceId *
ndn_IncomingFaceId_getFirstHeader(const struct ndn_LpPacket *lpPacket);

/**
 * Add an ndn_IncomingFaceId header field to lpPacket and initialize it.
 * @param lpPacket A pointer to the ndn_LpPacket struct to update.
 * @param incomingFaceId Set incomingFaceId to a pointer to the newly-added
 * ndn_IncomingFaceId struct.
 * @return 0 for success, or an error code if there is no more room in the
 * fieldHeaders array (nHeaderFields is already maxHeaderFields).
 */
ndn_Error
ndn_IncomingFaceId_add
  (struct ndn_LpPacket *lpPacket, struct ndn_IncomingFaceId **incomingFaceId);

#ifdef __cplusplus
}
#endif

#endif
