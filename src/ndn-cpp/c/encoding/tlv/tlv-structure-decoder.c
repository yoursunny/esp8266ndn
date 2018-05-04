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

#include "../../util/ndn_memory.h"
#include "tlv.h"
#include "tlv-decoder.h"
#include "tlv-structure-decoder.h"

void
ndn_TlvStructureDecoder_reset(struct ndn_TlvStructureDecoder *self)
{
  self->gotElementEnd = 0;
  self->offset = 0;
  self->state = ndn_TlvStructureDecoder_READ_TYPE;
  self->headerLength = 0;
  self->useHeaderBuffer = 0;
  self->nBytesToRead = 0;
}

ndn_Error
ndn_TlvStructureDecoder_findElementEnd(struct ndn_TlvStructureDecoder *self, const uint8_t *input, size_t inputLength)
{
  struct ndn_TlvDecoder decoder;

  if (self->gotElementEnd)
    // Someone is calling when we already got the end.
    return NDN_ERROR_success;

  ndn_TlvDecoder_initialize(&decoder, input, inputLength);

  while (1) {
    if (self->offset >= inputLength)
      // All the cases assume we have some input. Return and wait for more.
      return NDN_ERROR_success;

    switch (self->state) {
      case ndn_TlvStructureDecoder_READ_TYPE:
      {
        unsigned int firstOctet = (unsigned int)input[self->offset++];
        if (firstOctet < 253)
          // The value is simple, so we can skip straight to reading the length.
          self->state = ndn_TlvStructureDecoder_READ_LENGTH;
        else {
          // Set up to skip the type bytes.
          if (firstOctet == 253)
            self->nBytesToRead = 2;
          else if (firstOctet == 254)
            self->nBytesToRead = 4;
          else
            // value == 255.
            self->nBytesToRead = 8;

          self->state = ndn_TlvStructureDecoder_READ_TYPE_BYTES;
        }
        break;
      }
      case ndn_TlvStructureDecoder_READ_TYPE_BYTES:
      {
        size_t nRemainingBytes = inputLength - self->offset;
        if (nRemainingBytes < self->nBytesToRead) {
          // Need more.
          self->offset += nRemainingBytes;
          self->nBytesToRead -= nRemainingBytes;
          return NDN_ERROR_success;
        }
        // Got the type bytes.  Move on to read the length.
        self->offset += self->nBytesToRead;
        self->state = ndn_TlvStructureDecoder_READ_LENGTH;
        break;
      }
      case ndn_TlvStructureDecoder_READ_LENGTH:
      {
        unsigned int firstOctet = (unsigned int)input[self->offset++];
        if (firstOctet < 253) {
          // The value is simple, so we can skip straight to reading the value bytes.
          self->nBytesToRead = (size_t)firstOctet;
          if (self->nBytesToRead == 0) {
            // No value bytes to read.  We're finished.
            self->gotElementEnd = 1;
            return NDN_ERROR_success;
          }

          self->state = ndn_TlvStructureDecoder_READ_VALUE_BYTES;
        }
        else {
          // We need to read the bytes in the extended encoding of the length.
          if (firstOctet == 253)
            self->nBytesToRead = 2;
          else if (firstOctet == 254)
            self->nBytesToRead = 4;
          else
            // value == 255.
            self->nBytesToRead = 8;

          // We need to use firstOctet in the next state.
          self->firstOctet = firstOctet;
          self->state = ndn_TlvStructureDecoder_READ_LENGTH_BYTES;
        }
        break;
      }
      case ndn_TlvStructureDecoder_READ_LENGTH_BYTES:
      {
        size_t nRemainingBytes = inputLength - self->offset;
        if (!self->useHeaderBuffer && nRemainingBytes >= self->nBytesToRead) {
          uint64_t lengthVarNumber;
          ndn_Error error;

          // We don't have to use the headerBuffer.  Set nBytesToRead.
          ndn_TlvDecoder_seek(&decoder, self->offset);

          if ((error = ndn_TlvDecoder_readExtendedVarNumber(&decoder, self->firstOctet, &lengthVarNumber)))
            return error;
          // Update self->offset to the decoder's offset after reading.
          self->offset = decoder.offset;
          // Silently ignore if the length is larger than size_t.
          self->nBytesToRead = (size_t)lengthVarNumber;
        }
        else {
          size_t nNeededBytes;
          struct ndn_TlvDecoder bufferDecoder;
          uint64_t lengthVarNumber;
          ndn_Error error;

          self->useHeaderBuffer = 1;

          nNeededBytes = self->nBytesToRead - self->headerLength;
          if (nNeededBytes > nRemainingBytes) {
            // We can't get all of the header bytes from this input. Save in headerBuffer.
            if (self->headerLength + nRemainingBytes > sizeof(self->headerBuffer))
              return NDN_ERROR_cannot_store_more_header_bytes_than_the_size_of_headerBuffer;
            ndn_memcpy(self->headerBuffer + self->headerLength, input + self->offset, nRemainingBytes);
            self->offset += nRemainingBytes;
            self->headerLength += nRemainingBytes;

            return NDN_ERROR_success;
          }

          // Copy the remaining bytes into headerBuffer, read the length and set nBytesToRead.
          if (self->headerLength + nNeededBytes > sizeof(self->headerBuffer))
            return NDN_ERROR_cannot_store_more_header_bytes_than_the_size_of_headerBuffer;
          ndn_memcpy(self->headerBuffer + self->headerLength, input + self->offset, nNeededBytes);
          self->offset += nNeededBytes;

          // Use a local decoder just for the headerBuffer.
          ndn_TlvDecoder_initialize(&bufferDecoder, self->headerBuffer, sizeof(self->headerBuffer));
          if ((error = ndn_TlvDecoder_readExtendedVarNumber(&bufferDecoder, self->firstOctet, &lengthVarNumber)))
            return error;
          // Replace nBytesToRead with the length of the value.
          // Silently ignore if the length is larger than size_t.
          self->nBytesToRead = (size_t)lengthVarNumber;
        }

        if (self->nBytesToRead == 0) {
          // No value bytes to read.  We're finished.
          self->gotElementEnd = 1;
          return NDN_ERROR_success;
        }

        // Get ready to read the value bytes.
        self->state = ndn_TlvStructureDecoder_READ_VALUE_BYTES;
        break;
      }
      case ndn_TlvStructureDecoder_READ_VALUE_BYTES:
      {
        size_t nRemainingBytes = inputLength - self->offset;
        if (nRemainingBytes < self->nBytesToRead) {
          // Need more.
          self->offset += nRemainingBytes;
          self->nBytesToRead -= nRemainingBytes;
          return NDN_ERROR_success;
        }
        // Got the bytes. We're finished.
        self->offset += self->nBytesToRead;
        self->gotElementEnd = 1;
        return NDN_ERROR_success;
      }
      default:
        // We don't expect this to happen.
        return NDN_ERROR_findElementEnd_unrecognized_state;
    }
  }
}
