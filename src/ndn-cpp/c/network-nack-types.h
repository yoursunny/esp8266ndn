/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#ifndef NDN_NETWORK_NACK_TYPES_H
#define NDN_NETWORK_NACK_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ndn_NetworkNackReason specifies the reason in a NetworkNack packet. If the
 * reason code in the packet is not a recognized enum value, then we use
 * ndn_NetworkNackReason_OTHER_CODE and you can call getOtherReasonCode(). We do
 * this to keep the recognized reason values independent of packet encoding
 * formats.
 */
typedef enum {
  ndn_NetworkNackReason_NONE =        0,
  ndn_NetworkNackReason_CONGESTION = 50,
  ndn_NetworkNackReason_DUPLICATE = 100,
  ndn_NetworkNackReason_NO_ROUTE =  150,
  ndn_NetworkNackReason_OTHER_CODE = 0x7fff
} ndn_NetworkNackReason;

/**
 * An ndn_NetworkNack represents a network Nack packet and includes a Nack
 * reason.
 */
struct ndn_NetworkNack {
  ndn_NetworkNackReason reason;
  int otherReasonCode;
};

#ifdef __cplusplus
}
#endif

#endif
