/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#ifndef NDN_INCOMING_FACE_ID_TYPES_H
#define NDN_INCOMING_FACE_ID_TYPES_H

#include "../common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_IncomingFaceId represents the incoming face ID header field in an
 * NDNLPv2 packet.
 * http://redmine.named-data.net/projects/nfd/wiki/NDNLPv2
 */
struct ndn_IncomingFaceId {
  uint64_t faceId; /**< (uint64_t)-1 if not specified. */
};

#ifdef __cplusplus
}
#endif

#endif
