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

#ifndef NDN_TLV_KEY_LOCATOR_H
#define NDN_TLV_KEY_LOCATOR_H

#include "../../key-locator.h"
#include "tlv-encoder.h"
#include "tlv-decoder.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs in the body of the KeyLocator value.
 * @param context This is the ndn_KeyLocator struct pointer which was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_encodeTlvKeyLocatorValue(const void *context, struct ndn_TlvEncoder *encoder);

/**
 * Expect the next element to be a TLV KeyLocator and decode into the ndn_KeyLocator struct.
 * @param expectedType The expected type code of the TLV, e.g. ndn_Tlv_KeyLocator
 * or ndn_Tlv_PublisherPublicKeyLocator.
 * @param keyLocator A pointer to the ndn_KeyLocator struct.
 * @param decoder A pointer to the ndn_TlvDecoder struct.
 * @return 0 for success, else an error code, including if the next element is
 * not expectedType.
 */
ndn_Error
ndn_decodeTlvKeyLocator
  (unsigned int expectedType, struct ndn_KeyLocator *keyLocator,
   struct ndn_TlvDecoder *decoder);

#ifdef  __cplusplus
}
#endif

#endif
