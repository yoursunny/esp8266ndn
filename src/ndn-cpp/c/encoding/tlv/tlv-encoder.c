/**
 * Copyright (C) 2014-2018 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * Derived from tlv.hpp by Alexander Afanasyev <alexander.afanasyev@ucla.edu>
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
#include "../../util/endian.h"
#include "tlv-encoder.h"

/**
 * Call ndn_DynamicUInt8Array_ensureLength to ensure that there is enough room in the output, and copy
 * array to the output.  This does not write a header.  This assumes self->enableOutput is 1 and always writes.
 * @param self pointer to the ndn_TlvEncoder struct
 * @param array the array to copy
 * @param arrayLength the length of the array
 * @return 0 for success, else an error code.
 */
static ndn_Error
writeArrayEnabled(struct ndn_TlvEncoder *self, const uint8_t *array, size_t arrayLength)
{
  ndn_Error error;
  if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + arrayLength)))
    return error;

  ndn_memcpy(self->output->array + self->offset, array, arrayLength);
  self->offset += arrayLength;

  return NDN_ERROR_success;
}

ndn_Error
ndn_TlvEncoder_writeVarNumberEnabled(struct ndn_TlvEncoder *self, uint64_t varNumber)
{
  ndn_Error error;
  uint8_t *p;

  if (varNumber < 253) {
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + 1)))
      return error;
    self->output->array[self->offset] = (uint8_t)varNumber;
    self->offset += 1;
  }
  else if (varNumber <= 0xffff) {
    uint16_t beValue;
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + 3)))
      return error;
    self->output->array[self->offset] = 253;

#if NDN_CPP_HAVE_ENDIAN_H
    beValue = htobe16((uint16_t)varNumber);
    ndn_memcpy(self->output->array + self->offset + 1, (uint8_t *)&beValue, 2);
#else
    p = self->output->array + self->offset + 1;
    *(p++) = (varNumber >> 8) & 0xff;
    *p     = varNumber & 0xff;
#endif
    self->offset += 3;
  }
  else if (varNumber <= 0xffffffff) {
    uint32_t beValue;
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + 5)))
      return error;
    self->output->array[self->offset] = 254;

#if NDN_CPP_HAVE_ENDIAN_H
    beValue = htobe32((uint32_t)varNumber);
    ndn_memcpy(self->output->array + self->offset + 1, (uint8_t *)&beValue, 4);
#else
    p = self->output->array + self->offset + 1;
    *(p++) = (varNumber >> 24) & 0xff;
    *(p++) = (varNumber >> 16) & 0xff;
    *(p++) = (varNumber >> 8) & 0xff;
    *p     = varNumber & 0xff;
#endif
    self->offset += 5;
  }
  else {
    uint64_t beValue;
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + 9)))
      return error;
    self->output->array[self->offset] = 255;

#if NDN_CPP_HAVE_ENDIAN_H
    beValue = htobe64(varNumber);
    ndn_memcpy(self->output->array + self->offset + 1, (uint8_t *)&beValue, 8);
#else
    p = self->output->array + self->offset + 1;
    *(p++) = (varNumber >> 56) & 0xff;
    *(p++) = (varNumber >> 48) & 0xff;
    *(p++) = (varNumber >> 40) & 0xff;
    *(p++) = (varNumber >> 32) & 0xff;
    *(p++) = (varNumber >> 24) & 0xff;
    *(p++) = (varNumber >> 16) & 0xff;
    *(p++) = (varNumber >> 8) & 0xff;
    *p     = varNumber & 0xff;
#endif
    self->offset += 9;
  }

  return NDN_ERROR_success;
}

ndn_Error
ndn_TlvEncoder_writeNonNegativeIntegerEnabled(struct ndn_TlvEncoder *self, uint64_t value)
{
  ndn_Error error;
  uint8_t *p;

  if (value <= 0xff) {
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + 1)))
      return error;
    self->output->array[self->offset] = (uint8_t)value;
    self->offset += 1;
  }
  else if (value <= 0xffff) {
    uint16_t beValue;
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + 2)))
      return error;

#if NDN_CPP_HAVE_ENDIAN_H
    beValue = htobe16((uint16_t)value);
    ndn_memcpy(self->output->array + self->offset, (uint8_t *)&beValue, 2);
#else
    p = self->output->array + self->offset;
    *(p++) = (value >> 8) & 0xff;
    *p     = value & 0xff;
#endif
    self->offset += 2;
  }
  else if (value <= 0xffffffff) {
    uint32_t beValue;
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + 4)))
      return error;

#if NDN_CPP_HAVE_ENDIAN_H
    beValue = htobe32((uint32_t)value);
    ndn_memcpy(self->output->array + self->offset, (uint8_t *)&beValue, 4);
#else
    p = self->output->array + self->offset;
    *(p++) = (value >> 24) & 0xff;
    *(p++) = (value >> 16) & 0xff;
    *(p++) = (value >> 8) & 0xff;
    *p     = value & 0xff;
#endif
    self->offset += 4;
  }
  else {
    uint64_t beValue;
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, self->offset + 8)))
      return error;

#if NDN_CPP_HAVE_ENDIAN_H
    beValue = htobe64(value);
    ndn_memcpy(self->output->array + self->offset, (uint8_t *)&beValue, 8);
#else
    p = self->output->array + self->offset;
    *(p++) = (value >> 56) & 0xff;
    *(p++) = (value >> 48) & 0xff;
    *(p++) = (value >> 40) & 0xff;
    *(p++) = (value >> 32) & 0xff;
    *(p++) = (value >> 24) & 0xff;
    *(p++) = (value >> 16) & 0xff;
    *(p++) = (value >> 8) & 0xff;
    *p     = value & 0xff;
#endif
    self->offset += 8;
  }

  return NDN_ERROR_success;
}

ndn_Error
ndn_TlvEncoder_writeArrayEnabled
  (struct ndn_TlvEncoder *self, const uint8_t *array, size_t arrayLength)
{
  // This just provides a public version in the header. Call the static version.
  return writeArrayEnabled(self, array, arrayLength);
}

ndn_Error
ndn_TlvEncoder_writeBlobTlvEnabled
  (struct ndn_TlvEncoder *self, unsigned int type, const struct ndn_Blob *value)
{
  ndn_Error error;
  if ((error = ndn_TlvEncoder_writeTypeAndLength(self, type, value->length)))
    return error;
  if ((error = writeArrayEnabled(self, value->value, value->length)))
    return error;

  return NDN_ERROR_success;
}

ndn_Error
ndn_TlvEncoder_writeNestedTlv
  (struct ndn_TlvEncoder *self, unsigned int type,
   ndn_Error (*writeValue)(const void *context, struct ndn_TlvEncoder *encoder),
   const void *context, int omitZeroLength)
{
  ndn_Error error;
  size_t valueLength;
  int originalEnableOutput = self->enableOutput;

  // Make a first pass to get the value length by setting enableOutput = 0.
  size_t saveOffset = self->offset;
  self->enableOutput = 0;
  if ((error = writeValue(context, self)))
    return error;
  valueLength = self->offset - saveOffset;

  if (omitZeroLength && valueLength == 0) {
    // Omit the optional TLV.
    self->enableOutput = originalEnableOutput;
    return NDN_ERROR_success;
  }

  if (originalEnableOutput) {
    // Restore the offset and enableOutput.
    self->offset = saveOffset;
    self->enableOutput = 1;

    // Now, write the output.
    if ((error = ndn_TlvEncoder_writeTypeAndLength(self, type, valueLength)))
      return error;
    if ((error = writeValue(context, self)))
      return error;
  }
  else
    // The output was originally disabled. Just advance offset further by the type and length.
    ndn_TlvEncoder_writeTypeAndLength(self, type, valueLength);

  return NDN_ERROR_success;
}
