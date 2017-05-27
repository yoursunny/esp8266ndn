/**
 * Copyright (C) 2016-2017 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-cxx packet.hpp https://github.com/named-data/ndn-cxx/blob/master/src/lp/packet.hpp
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

#ifndef NDN_LP_PACKET_TYPES_H
#define NDN_LP_PACKET_TYPES_H

#include "../util/blob-types.h"
#include "incoming-face-id-types.h"
#include "../network-nack-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ndn_LpPacketHeaderFieldType is used only in the ndn_LpPacketHeaderField union.
 */
typedef enum {
  ndn_LpPacketHeaderFieldType_INCOMING_FACE_ID = 1,
  ndn_LpPacketHeaderFieldType_NETWORK_NACK = 2
} ndn_LpPacketHeaderFieldType;

/**
 * An ndn_LpPacketHeaderField holds a type and a union with values for the
 * different types of LP packet header field.
 */
struct ndn_LpPacketHeaderField {
  ndn_LpPacketHeaderFieldType type;
  union {
    struct ndn_IncomingFaceId incomingFaceId;
    struct ndn_NetworkNack networkNack;
  };
};

/**
 * An ndn_LpPacket holds a list of LP packet header fields and an optional
 * fragment wire encoding.
 */
struct ndn_LpPacket {
  struct ndn_Blob fragmentWireEncoding; /**< A Blob whose value is a pointer to
    * a pre-allocated buffer for the fragment wire encoding. */
  struct ndn_LpPacketHeaderField *headerFields; /**< pointer to the array of ndn_LpPacketHeaderField. */
  size_t maxHeaderFields; /**< the number of elements in the allocated headerFields array. */
  size_t nHeaderFields;   /**< the number of header fields in the packet. */
};

#ifdef __cplusplus
}
#endif

#endif
