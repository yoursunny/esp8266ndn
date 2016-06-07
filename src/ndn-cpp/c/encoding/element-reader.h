/**
 * Copyright (C) 2013-2016 Regents of the University of California.
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

#ifndef NDN_ELEMENT_READER_H
#define NDN_ELEMENT_READER_H

#include "element-listener.h"
#include "tlv/tlv-structure-decoder.h"
#include "../util/dynamic-uint8-array.h"
#include <ndn-cpp/c/encoding/element-reader-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_ElementReader struct with the elementListener and a buffer for saving partial data.
 * @param self pointer to the ndn_ElementReader struct
 * @param elementListener pointer to the ndn_ElementListener used by
 * ndn_ElementReader_onReceivedData. If this is 0, you can set it later with
 * ndn_ElementReader_reset.
 * @param buffer A pointer to a ndn_DynamicUInt8Array struct which is used to
 * save data before calling the elementListener. The struct must remain valid
 * during the entire life of this ndn_ElementReader. If the buffer->realloc
 * function pointer is 0, its array must be large enough to save a full element,
 * perhaps MAX_NDN_PACKET_SIZE bytes.
 */
static __inline void ndn_ElementReader_initialize
  (struct ndn_ElementReader *self, struct ndn_ElementListener *elementListener,
   struct ndn_DynamicUInt8Array *buffer)
{
  self->elementListener = elementListener;
  ndn_TlvStructureDecoder_initialize(&self->tlvStructureDecoder);
  self->partialData = buffer;
  self->usePartialData = 0;
}

/**
 * Reset the state of this ElementReader to begin reading new data and use the
 * given elementListener. Keep using the buffer provided to
 * ndn_ElementReader_initialize.
 * @param self pointer to the ndn_ElementReader struct.
 * @param elementListener pointer to the ndn_ElementListener used by
 * ndn_ElementReader_onReceivedData.
 */
static __inline void ndn_ElementReader_reset
  (struct ndn_ElementReader *self, struct ndn_ElementListener *elementListener)
{
  self->elementListener = elementListener;
  ndn_TlvStructureDecoder_reset(&self->tlvStructureDecoder);
  self->usePartialData = 0;
}

/**
 * Continue to read data until the end of an element, then call (*elementListener->onReceivedElement)(element, elementLength).
 * The buffer passed to onReceivedElement is only valid during this call.  If you need the data later, you must copy.
 * @param self pointer to the ndn_ElementReader struct
 * @param data pointer to the buffer with the incoming element's bytes
 * @param dataLength length of data
 * @return 0 for success, else an error code
 */
ndn_Error ndn_ElementReader_onReceivedData
  (struct ndn_ElementReader *self, const uint8_t *data, size_t dataLength);

#ifdef __cplusplus
}
#endif

#endif
