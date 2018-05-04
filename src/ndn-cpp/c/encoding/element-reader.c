/**
 * Copyright (C) 2013-2018 Regents of the University of California.
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

#include "tlv/tlv.h"
#include "element-reader.h"

ndn_Error ndn_ElementReader_onReceivedData
  (struct ndn_ElementReader *self, const uint8_t *data, size_t dataLength)
{
  // Process multiple objects in the data.
  while(1) {
    ndn_Error error;
    int gotElementEnd;
    size_t offset;

    if (!self->usePartialData) {
      // This is the beginning of an element.
      if (dataLength <= 0)
        // Wait for more data.
        return NDN_ERROR_success;
    }

    // Scan the input to check if a whole TLV element has been read.
    ndn_TlvStructureDecoder_seek(&self->tlvStructureDecoder, 0);
    error = ndn_TlvStructureDecoder_findElementEnd(&self->tlvStructureDecoder, data, dataLength);
    gotElementEnd = self->tlvStructureDecoder.gotElementEnd;
    offset = self->tlvStructureDecoder.offset;

    if (error) {
      // Reset to read a new element on the next call.
      self->usePartialData = 0;
      ndn_TlvStructureDecoder_initialize(&self->tlvStructureDecoder);

      return error;
    }

    if (gotElementEnd) {
      const uint8_t *element = 0;
      size_t elementLength;

      if (!self->elementListener)
        return NDN_ERROR_ElementReader_ElementListener_is_not_specified;

      // Got the remainder of an element.  Report to the caller.
      if (self->usePartialData) {
        if (self->gotPartialDataError) {
          // We returned an error allocating the partialData, so it is not
          // valid. Therefore, don't send it to the callback.
        }
        else {
          // We have partial data from a previous call, so append this data and point to partialData.
          if ((error = ndn_DynamicUInt8Array_copy(self->partialData, data, offset, self->partialDataLength)))
            return error;
          self->partialDataLength += offset;

          element = self->partialData->array;
          elementLength = self->partialDataLength;
        }

        // Assume we don't need to use partialData anymore until needed.
        self->usePartialData = 0;
      }
      else {
        // We are not using partialData, so just point to the input data buffer.
        element = data;
        elementLength = offset;
      }

      // Reset to read a new object. Do this before calling onReceivedElement
      // in case it throws an exception.
      data += offset;
      dataLength -= offset;
      ndn_TlvStructureDecoder_initialize(&self->tlvStructureDecoder);

      if (element)
        (*self->elementListener->onReceivedElement)
          (self->elementListener, element, elementLength);
      if (dataLength == 0)
        // No more data in the packet.
        return NDN_ERROR_success;

      // else loop back to decode.
    }
    else {
      // Save remaining data for a later call.
      if (!self->usePartialData) {
        self->usePartialData = 1;
        self->gotPartialDataError = 0;
        self->partialDataLength = 0;
      }

      if (!self->gotPartialDataError) {
        if (self->partialDataLength + dataLength > MAX_NDN_PACKET_SIZE) {
          // Reset to read a new element on the next call.
          self->usePartialData = 0;
          ndn_TlvStructureDecoder_initialize(&self->tlvStructureDecoder);

          return NDN_ERROR_ElementReader_The_incoming_packet_exceeds_the_maximum_limit_getMaxNdnPacketSize;
        }

        if ((error = ndn_DynamicUInt8Array_copy
             (self->partialData, data, dataLength, self->partialDataLength))) {
          // Set gotPartialDataError so we won't call onReceivedElement with invalid data.
          self->gotPartialDataError = 1;
          return error;
        }
        self->partialDataLength += dataLength;
      }

      return NDN_ERROR_success;
    }
  }
}
