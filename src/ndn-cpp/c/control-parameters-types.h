/**
 * Copyright (C) 2015-2018 Regents of the University of California.
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

#ifndef NDN_CONTROL_PARAMETERS_TYPES_H
#define NDN_CONTROL_PARAMETERS_TYPES_H

#include "common.h"
#include "name-types.h"
#include "forwarding-flags.h"
#include "util/blob-types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_NfdForwardingFlags_CHILD_INHERIT = 1,
  ndn_NfdForwardingFlags_CAPTURE       = 2,
} ndn_NfdForwardingFlags;

/**
 * An ndn_ControlParameters holds fields for a ControlParameters which is
 * used in the command interest such as to register a prefix with a forwarder.
 */
struct ndn_ControlParameters {
  int hasName;
  struct ndn_Name name;              /**< Only used if hasName */
  int faceId;                        /**< -1 for none. */
  struct ndn_Blob uri;               /**< A Blob whose value is a pointer to pre-allocated buffer.
                                          0 for none. */
  int localControlFeature;           /**< -1 for none. */
  int origin;                        /**< -1 for none. */
  int cost;                          /**< -1 for none. */
  struct ndn_ForwardingFlags flags;
  struct ndn_Name strategy;          /**< nComponents == 0 for none. */
  ndn_Milliseconds expirationPeriod; /**< -1 for none. */
};

#ifdef __cplusplus
}
#endif

#endif
