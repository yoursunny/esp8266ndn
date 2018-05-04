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

#ifndef NDN_CONTROL_RESPONSE_TYPES_H
#define NDN_CONTROL_RESPONSE_TYPES_H

#include "control-parameters-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_ControlResponse struct holds a status code, status text and other
 * fields for a ControlResponse which is used, for example, in the response from
 * sending a register prefix control command to a forwarder.
 */
struct ndn_ControlResponse {
  int statusCode;             /**< -1 for none. */
  struct ndn_Blob statusText; /**< A Blob whose value is a pointer to pre-allocated buffer.
                                   0 for none. */
  int hasBodyAsControlParameters;
  struct ndn_ControlParameters bodyAsControlParameters;
};

#ifdef __cplusplus
}
#endif

#endif
