/**
 * Copyright (C) 2014-2016 Regents of the University of California.
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

#ifndef NDN_TLV_DECODER_H
#define NDN_TLV_DECODER_H

#include <ndn-cpp/c/common.h>
#include <ndn-cpp/c/errors.h>
#include "../../util/blob.h"
#include "tlv.h"

#ifdef  __cplusplus
extern "C" {
#endif

struct ndn_TlvDecoder {
  const uint8_t *input;
  size_t inputLength;
  size_t offset;
};

/**
 * Initialize an ndn_TlvDecoder struct to decode the input.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param input A pointer to the input buffer to decode.
 * @param inputLength The number of bytes in input.
 */
static __inline void
ndn_TlvDecoder_initialize(struct ndn_TlvDecoder *self, const uint8_t *input, size_t inputLength)
{
  self->input = input;
  self->inputLength = inputLength;
  self->offset = 0;
}

/**
 * A private function to do the work of readVarNumber, given the firstOctet which is >= 253.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param firstOctet The first octet which is >= 253, used to decode the remaining bytes.
 * @param varNumber Return the VAR-NUMBER.
 * @return
 */
ndn_Error
ndn_TlvDecoder_readExtendedVarNumber(struct ndn_TlvDecoder *self, unsigned int firstOctet, uint64_t *varNumber);

/**
 * Decode VAR-NUMBER in NDN-TLV and set varNumber. Update offset.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param varNumber Return the VAR-NUMBER.
 * @return 0 for success, else an error code for read past the end of the input.
 */
static __inline ndn_Error
ndn_TlvDecoder_readVarNumber(struct ndn_TlvDecoder *self, uint64_t *varNumber)
{
  unsigned int firstOctet;

  // Read the first octet inline.
  if (self->offset >= self->inputLength)
    return NDN_ERROR_read_past_the_end_of_the_input;

  firstOctet = (unsigned int)self->input[self->offset++];
  if (firstOctet < 253) {
    // The value is simple, so we can return it inline.
    *varNumber = (uint64_t)firstOctet;
    return NDN_ERROR_success;
  }
  else
    return ndn_TlvDecoder_readExtendedVarNumber(self, firstOctet, varNumber);
}

/**
 * Decode the type and length from self's input starting at offset, expecting the type to be expectedType.
 * Update offset.  Also make sure the decoded length does not exceed the number of bytes remaining in the input.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType The expected type.
 * @param length Return the length of the TLV.
 * @return 0 for success, else an error code, including an error if not the expected type or if the decoded length
 * exceeds the number of bytes remaining.
 */
static __inline ndn_Error
ndn_TlvDecoder_readTypeAndLength
  (struct ndn_TlvDecoder *self, unsigned int expectedType, size_t *length)
{
  ndn_Error error;
  uint64_t type;
  uint64_t lengthVarNumber;

  if ((error = ndn_TlvDecoder_readVarNumber(self, &type)))
    return error;

  if (type != (uint64_t)expectedType)
    return NDN_ERROR_did_not_get_the_expected_TLV_type;

  if ((error = ndn_TlvDecoder_readVarNumber(self, &lengthVarNumber)))
    return error;

  // Silently ignore if the length is larger than size_t.
  *length = (size_t)lengthVarNumber;
  if (self->offset + *length > self->inputLength)
    return NDN_ERROR_TLV_length_exceeds_buffer_length;

  return NDN_ERROR_success;
}

/**
 * Decode the type and length from self's input starting at offset, expecting the type to be expectedType.
 * Update offset.  Also make sure the decoded length does not exceed the number of bytes remaining in the input.
 * Set endOffset to the offset of the end of this parent TLV, which is used in decoding optional nested TLVs.
 * After reading all nested TLVs, call ndn_TlvDecoder_finishNestedTlvs.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType The expected type.
 * @param endOffset Return the offset of the end of the parent TLV.
 * @return 0 for success, else an error code, including an error if not the expected type or if the decoded length
 * exceeds the number of bytes remaining.
 */
static __inline ndn_Error
ndn_TlvDecoder_readNestedTlvsStart(struct ndn_TlvDecoder *self, unsigned int expectedType, size_t *endOffset)
{
  ndn_Error error;
  size_t length;
  if ((error = ndn_TlvDecoder_readTypeAndLength(self, expectedType, &length)))
    return error;
  *endOffset = self->offset + length;

  return NDN_ERROR_success;
}

/**
 * A private function used by finishNestedTlvs to skip the remaining nested
 * TLVs and to make sure the resulting offset equals endOffset.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param endOffset The offset of the end of the parent TLV, returned by
 * ndn_TlvDecoder_readNestedTlvsStart.
 * @return 0 for success, else an error code if the TLV length does not equal
 * the total length of nested TLVs
 */
ndn_Error
ndn_TlvDecoder_skipRemainingNestedTlvs
  (struct ndn_TlvDecoder *self, size_t endOffset);

/**
 * Call this after reading all nested TLVs to skip any remaining unrecognized
 * TLVs and to check if the offset after the final nested TLV matches the
 * endOffset returned by ndn_TlvDecoder_readNestedTlvsStart.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param endOffset The offset of the end of the parent TLV, returned by ndn_TlvDecoder_readNestedTlvsStart.
 * @return 0 for success, else an error code if the TLV length does not equal the total length of nested TLVs
 */
static __inline ndn_Error
ndn_TlvDecoder_finishNestedTlvs(struct ndn_TlvDecoder *self, size_t endOffset)
{
  // Check the simple, expected case inline.
  if (self->offset != endOffset)
    return ndn_TlvDecoder_skipRemainingNestedTlvs(self, endOffset);

  return NDN_ERROR_success;
}

/**
 * Decode the type from self's input starting at offset, and if it is the expectedType,
 * then set gotExpectedType to 1, else 0.  However, if self->offset is greater than or equal to endOffset,
 * then set gotExpectedType to 0 and don't try to read the type.
 * Do not update offset, including if returning an error.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType the expected type.
 * @param endOffset The offset of the end of the parent TLV.
 * @param gotExpectedType Return a 1 if got the expected type, else 0
 * @return 0 for success, else an error code for read past the end of the input.
 */
static __inline ndn_Error
ndn_TlvDecoder_peekType(struct ndn_TlvDecoder *self, unsigned int expectedType, size_t endOffset, int *gotExpectedType)
{
  if (self->offset >= endOffset)
    // No more sub TLVs to look at.
    *gotExpectedType = 0;
  else {
    size_t saveOffset = self->offset;
    uint64_t type;
    ndn_Error error = ndn_TlvDecoder_readVarNumber(self, &type);
    // Restore offset.
    self->offset = saveOffset;
    if (error)
      return error;

    *gotExpectedType = (type == expectedType ? 1 : 0);
  }

  return NDN_ERROR_success;
}

/**
 * A private function to do the work of readNonNegativeInteger, assuming the length != 1.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param length The number of bytes in the encoded integer.
 * @param value Return the integer.
 * @return
 */
ndn_Error
ndn_TlvDecoder_readExtendedNonNegativeInteger(struct ndn_TlvDecoder *self, size_t length, uint64_t *value);

/**
 * Decode a non-negative integer in NDN-TLV and set value. Update offset by length.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param length The number of bytes in the encoded integer.
 * @param value Return the integer.
 * @return 0 for success, else an error code for read past the end of the input.
 */
static __inline ndn_Error
ndn_TlvDecoder_readNonNegativeInteger(struct ndn_TlvDecoder *self, size_t length, uint64_t *value)
{
  if (length == 1) {
    // Read the simple integer inline.
    if (self->offset >= self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;

    *value = (uint64_t)self->input[self->offset++];
    return NDN_ERROR_success;
  }
  else
    return ndn_TlvDecoder_readExtendedNonNegativeInteger(self, length, value);
}

/**
 * Decode the type and length from self's input starting at offset, expecting the type to be expectedType.
 * Then decode a non-negative integer in NDN-TLV and set value.  Update offset.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType The expected type.
 * @param value Return the integer.
 * @return 0 for success, else an error code, including an error if not the expected type or if the decoded length
 * exceeds the number of bytes remaining.
 */
static __inline ndn_Error
ndn_TlvDecoder_readNonNegativeIntegerTlv(struct ndn_TlvDecoder *self, unsigned int expectedType, uint64_t *value)
{
  ndn_Error error;
  size_t length;
  if ((error = ndn_TlvDecoder_readTypeAndLength(self, expectedType, &length)))
    return error;
  if ((error = ndn_TlvDecoder_readNonNegativeInteger(self, length, value)))
    return error;

  return NDN_ERROR_success;
}

/**
 * Peek at the next TLV, and if it has the expectedType then call ndn_TlvDecoder_readNonNegativeIntegerTlv.
 * Otherwise, set value to -1.  However, if self->offset is greater than or equal to endOffset,
 * then set value to -1 and don't try to read the type.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType The expected type.
 * @param endOffset The offset of the end of the parent TLV.
 * @param value Return the integer (converted to int) or -1 if the next TLV doesn't have the expected type.
 * @return 0 for success, else an error code, including if the decoded length exceeds the number of bytes remaining.
 */
static __inline ndn_Error
ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
  (struct ndn_TlvDecoder *self, unsigned int expectedType, size_t endOffset,
   int *value)
{
  int gotExpectedType;
  ndn_Error error;
  uint64_t unsignedValue;

  if ((error = ndn_TlvDecoder_peekType
       (self, expectedType, endOffset, &gotExpectedType)))
    return error;

  if (!gotExpectedType) {
    *value = -1;
    return NDN_ERROR_success;
  }

  if ((error = ndn_TlvDecoder_readNonNegativeIntegerTlv
       (self, expectedType, &unsignedValue)))
    return error;

  *value = (int)unsignedValue;
  return NDN_ERROR_success;
}

/**
 * Peek at the next TLV, and if it has the expectedType then call ndn_TlvDecoder_readNonNegativeIntegerTlv
 * and return the value as a double. Otherwise, set value to -1.
 * However, if self->offset is greater than or equal to endOffset, then set value to -1 and don't try to read the type.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType The expected type.
 * @param endOffset The offset of the end of the parent TLV.
 * @param value Return the integer (converted to double) or -1 if the next TLV doesn't have the expected type.
 * @return 0 for success, else an error code, including if the decoded length exceeds the number of bytes remaining.
 */
static __inline ndn_Error
ndn_TlvDecoder_readOptionalNonNegativeIntegerTlvAsDouble
  (struct ndn_TlvDecoder *self, unsigned int expectedType, size_t endOffset,
   double *value)
{
  int gotExpectedType;
  ndn_Error error;
  uint64_t unsignedValue;

  if ((error = ndn_TlvDecoder_peekType
       (self, expectedType, endOffset, &gotExpectedType)))
    return error;

  if (!gotExpectedType) {
    *value = -1;
    return NDN_ERROR_success;
  }

  if ((error = ndn_TlvDecoder_readNonNegativeIntegerTlv
       (self, expectedType, &unsignedValue)))
    return error;

  *value = (double)unsignedValue;
  return NDN_ERROR_success;
}

/**
 * Decode the type and length from self's input starting at offset, expecting the type to be expectedType.
 * Then return the value and length in the ndn_Blob.  Update offset.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType The expected type.
 * @param value Set value.value to a pointer to the binary data inside self's input buffer and set value.length to the length.
 * @return 0 for success, else an error code, including an error if not the expected type or if the decoded length
 * exceeds the number of bytes remaining.
 */
static __inline ndn_Error
ndn_TlvDecoder_readBlobTlv
  (struct ndn_TlvDecoder *self, unsigned int expectedType, struct ndn_Blob *value)
{
  ndn_Error error;
  if ((error = ndn_TlvDecoder_readTypeAndLength(self, expectedType, &value->length)))
    return error;

  // ndn_TlvDecoder_readTypeAndLength already checked if the value->length exceeds the input buffer.
  value->value = self->input + self->offset;
  self->offset += value->length;

  return NDN_ERROR_success;
}

/**
 * Peek at the next TLV, and if it has the expectedType then call
 * ndn_TlvDecoder_readBlobTlv.  Otherwise, set value and valueLength to 0.
 * However, if self->offset is greater than or equal to endOffset,
 * then set value and valueLength to 0 and don't try to read the type.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType The expected type.
 * @param endOffset The offset of the end of the parent TLV.
 * @param value Set value.value to a pointer to the binary data inside self's
 * input buffer and set value.length to the length.
 * @return 0 for success, else an error code, including if the decoded length
 * exceeds the number of bytes remaining.
 */
static __inline ndn_Error
ndn_TlvDecoder_readOptionalBlobTlv
  (struct ndn_TlvDecoder *self, unsigned int expectedType, size_t endOffset,
   struct ndn_Blob *value)
{
  int gotExpectedType;
  ndn_Error error;

  if ((error = ndn_TlvDecoder_peekType
       (self, expectedType, endOffset, &gotExpectedType)))
    return error;

  if (!gotExpectedType) {
    value->value = 0;
    value->length = 0;
    return NDN_ERROR_success;
  }

  if ((error = ndn_TlvDecoder_readBlobTlv(self, expectedType, value)))
    return error;

  return NDN_ERROR_success;
}

/**
 * Peek at the next TLV, and if it has the expectedType then set value to 1 and read a type and value,
 * ignoring the value. Otherwise, set value to 0.  However, if self->offset is greater than or equal to endOffset,
 * then set value to 0 and don't try to read the type.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param expectedType The expected type.
 * @param endOffset The offset of the end of the parent TLV.
 * @param value Return 1 or 0 if the next TLV doesn't have the expected type.
 * @return 0 for success, else an error code, including if the decoded length exceeds the number of bytes remaining.
 */
static __inline ndn_Error
ndn_TlvDecoder_readBooleanTlv
  (struct ndn_TlvDecoder *self, unsigned int expectedType, size_t endOffset, int *value)
{
  int gotExpectedType;
  ndn_Error error;
  if ((error = ndn_TlvDecoder_peekType(self, expectedType, endOffset, &gotExpectedType)))
    return error;

  if (!gotExpectedType)
    *value = 0;
  else {
    size_t length;
    if ((error = ndn_TlvDecoder_readTypeAndLength(self, expectedType, &length)))
      return error;
    // We expect the length to be 0, but update offset anyway.
    self->offset += length;

    *value = 1;
  }

  return NDN_ERROR_success;
}

/**
 * Set the offset into the input, used for the next read.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param offset The new offset.
 */
static __inline void ndn_TlvDecoder_seek(struct ndn_TlvDecoder *self, size_t offset)
{
  self->offset = offset;
}

/**
 * Set slice to a slice of the input for the given offset range.
 * @param self A pointer to the ndn_TlvDecoder struct.
 * @param beginOffset The offset in the input of the beginning of the slice.
 * @param endOffset The offset in the input of the end of the slice.
 * @param value Set value.value to a pointer to the binary data inside self's input buffer and set value.length to the length.
 * @return 0 for success, else an error code, including an error if beginOffset
 * or endOffset exceeds the length of the input.
 */
static __inline ndn_Error
ndn_TlvDecoder_getSlice
  (struct ndn_TlvDecoder *self, size_t beginOffset, size_t endOffset,
   struct ndn_Blob *slice)
{
  if (beginOffset > self->inputLength || endOffset > self->inputLength)
    return NDN_ERROR_read_past_the_end_of_the_input;

  slice->value = self->input + beginOffset;
  if (beginOffset > endOffset)
    // We don't expect this to happen.
    slice->length = 0;
  else
    slice->length = endOffset - beginOffset;

  return NDN_ERROR_success;
}

#ifdef  __cplusplus
}
#endif

#endif
