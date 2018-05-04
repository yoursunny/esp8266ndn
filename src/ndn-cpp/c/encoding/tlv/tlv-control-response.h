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

#ifndef NDN_TLV_CONTROL_RESPONSE_H
#define NDN_TLV_CONTROL_RESPONSE_H

#include "../../control-response.h"
#include "tlv-encoder.h"
#include "tlv-decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode controlResponse as an NDN-TLV ControlResponse.
 * @param controlResponse A pointer to the
 * ndn_ControlResponse struct to encode.
 * @param encoder Pointer to the ndn_TlvEncoder struct which receives the
 * encoding.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_encodeTlvControlResponse
  (const struct ndn_ControlResponse *controlResponse,
   struct ndn_TlvEncoder *encoder);

/**
 * Expect the next element to be a TLV ControlResponse and decode into the
 * ndn_ControlResponse struct.
 * @param controlResponse A pointer to the ndn_ControlResponse struct.
 * @param decoder A pointer to the ndn_TlvDecoder struct.
 * @return 0 for success, else an error code, including if the next element is
 * not ControlResponse.
 */
ndn_Error
ndn_decodeTlvControlResponse
  (struct ndn_ControlResponse *controlResponse, struct ndn_TlvDecoder *decoder);

#ifdef __cplusplus
}
#endif

#endif
