/**
 * Copyright (C) 2014-2016 Regents of the University of California.
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

#ifndef NDN_TLV_STRUCTURE_DECODER_H
#define NDN_TLV_STRUCTURE_DECODER_H

#include <ndn-cpp/c/common.h>
#include <ndn-cpp/c/errors.h>
#include <ndn-cpp/c/encoding/element-reader-types.h>

#ifdef  __cplusplus
extern "C" {
#endif

enum {
  ndn_TlvStructureDecoder_READ_TYPE,
  ndn_TlvStructureDecoder_READ_TYPE_BYTES,
  ndn_TlvStructureDecoder_READ_LENGTH,
  ndn_TlvStructureDecoder_READ_LENGTH_BYTES,
  ndn_TlvStructureDecoder_READ_VALUE_BYTES
};

/**
 * Reset this ndn_TlvStructureDecoder struct to the state created by
 * ndn_TlvStructureDecoder_initialize.
 * @param self A pointer to the ndn_TlvStructureDecoder struct.
 */
void
ndn_TlvStructureDecoder_reset(struct ndn_TlvStructureDecoder *self);

/**
 * Initialize a ndn_TlvStructureDecoder struct.
 * @param self A pointer to the ndn_TlvStructureDecoder struct.
 */
static __inline void
ndn_TlvStructureDecoder_initialize(struct ndn_TlvStructureDecoder *self)
{
  ndn_TlvStructureDecoder_reset(self);
}

/**
 * Continue scanning input starting from self->offset to find the element end.  On return, you must check
 *   self->gotElementEnd: If the end of the element which started at offset 0 is found,
 *   then self->gotElementEnd is 1 and self->offset is the length of the element.  Otherwise, self-gotElementEnd is 0
 *   which means you should read more into input and call again.
 * @param self A pointer to the ndn_TlvStructureDecoder struct.
 * @param input The input buffer. You have to pass in input each time because the buffer could be reallocated.
 * @param inputLength The number of bytes in input.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_TlvStructureDecoder_findElementEnd(struct ndn_TlvStructureDecoder *self, const uint8_t *input, size_t inputLength);

/**
 * Set the offset into the input, used for the next read.
 * @param self A pointer to the ndn_TlvStructureDecoder struct.
 * @param offset The new offset.
 */
static __inline void
ndn_TlvStructureDecoder_seek(struct ndn_TlvStructureDecoder *self, size_t offset)
{
  self->offset = offset;
}

#ifdef  __cplusplus
}
#endif

#endif
