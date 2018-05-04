/**
 * Copyright (C) 2014-2018 Regents of the University of California.
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

#ifndef NDN_TLV_NAME_H
#define NDN_TLV_NAME_H

#include "../../name.h"
#include "tlv-encoder.h"
#include "tlv-decoder.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * Encode the name component to the encoder as NDN-TLV. This handles different
 * component types such as ImplicitSha256DigestComponent.
 * @param component A pointer to the name component to encode.
 * @param encoder A pointer to the ndn_TlvEncoder struct which receives
 * the encoding.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_encodeTlvNameComponent
  (const struct ndn_NameComponent *component, struct ndn_TlvEncoder *encoder);

/**
 * Decode the next element as a TLV NameComponent into the ndn_NameComponent
 * struct. This handles different component types such as
 * ImplicitSha256DigestComponent.
 * @param component A pointer to the ndn_NameComponent struct.
 * @param decoder A pointer to the ndn_TlvDecoder struct.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_decodeTlvNameComponent
  (struct ndn_NameComponent *component, struct ndn_TlvDecoder *decoder);

/**
 * Encode the name as NDN-TLV.
 * @param name A pointer to the name object to encode.
 * @param signedPortionBeginOffset Return the offset in the encoding of the
 * beginning of the signed portion. The signed portion starts from the first
 * name component and ends just before the final name component (which is
 * assumed to be a signature for a signed interest).
 * If you are not encoding in order to sign, you can ignore this returned value.
 * @param signedPortionEndOffset Return the offset in the encoding of the end of
 * the signed portion. The signed portion starts from the first
 * name component and ends just before the final name component (which is
 * assumed to be a signature for a signed interest).
 * If you are not encoding in order to sign, you can ignore this returned value.
 * @param encoder A pointer to the ndn_TlvEncoder struct which receives
 * the encoding.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_encodeTlvName
  (const struct ndn_Name *name, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_TlvEncoder *encoder);

/**
 * Expect the next element to be a TLV Name and decode into the ndn_Name struct.
 * @param name A pointer to the ndn_Name struct.
 * @param signedPortionBeginOffset Return the offset in the encoding of the
 * beginning of the signed portion. The signed portion starts from the first
 * name component and ends just before the final name component (which is
 * assumed to be a signature for a signed interest).
 * @param signedPortionEndOffset Return the offset in the encoding of the end
 * of the signed portion. The signed portion starts from the first
 * name component and ends just before the final name component (which is
 * assumed to be a signature for a signed interest).
 * @param decoder A pointer to the ndn_TlvDecoder struct.
 * @return 0 for success, else an error code, including if the next element is
 * not a Name.
 */
ndn_Error
ndn_decodeTlvName
  (struct ndn_Name *name, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_TlvDecoder *decoder);

#ifdef  __cplusplus
}
#endif

#endif
