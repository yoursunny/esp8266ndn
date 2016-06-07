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

#ifndef NDN_TLV_ENCODER_H
#define NDN_TLV_ENCODER_H

#include <math.h>
#include <ndn-cpp/c/errors.h>
#include "../../util/dynamic-uint8-array.h"
#include "../../util/blob.h"
#include "tlv.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An ndn_TlvEncoder struct is used by all the TLV encoding functions.  You should initialize it with
 * ndn_TlvEncoder_initialize.  You can set enableOutput to 0 to only advance self->offset without writing to output
 * as a way to pre-compute the length of child elements.
 */
struct ndn_TlvEncoder {
  struct ndn_DynamicUInt8Array *output; /**< A pointer to a ndn_DynamicUInt8Array which receives the encoded output. */
  size_t offset;                        /**< The offset into output.array for the next encoding. */
  int enableOutput;                     /**< If 0, then only advance offset without writing to output. */
};

/**
 * Initialize an ndn_TlvEncoder struct with the arguments for initializing the ndn_DynamicUInt8Array, and set
 * enableOutput to 1.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param output A pointer to a ndn_DynamicUInt8Array struct which receives the encoded output.  The struct must
 * remain valid during the entire life of this ndn_TlvEncoder. If the output->realloc
 * function pointer is null, its array must be large enough to receive the entire encoding.
 */
static __inline void
ndn_TlvEncoder_initialize(struct ndn_TlvEncoder *self, struct ndn_DynamicUInt8Array *output)
{
  self->output = output;
  self->offset = 0;
  self->enableOutput = 1;
}

/**
 * Set self->offset to the given offset. This is typically called immediately
 * after initialize to set the location in the output to put the encoding.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param offset The new value for self->offset.
 * @return 0 for success, else an error if this cannot ensure offset bytes in
 * the output buffer.
 */
static __inline ndn_Error
ndn_TlvEncoder_seek(struct ndn_TlvEncoder *self, size_t offset)
{
  ndn_Error error;
  if (self->enableOutput) {
    if ((error = ndn_DynamicUInt8Array_ensureLength(self->output, offset)))
      return error;
  }

  self->offset = offset;
  return NDN_ERROR_success;
}

/**
 * Return the number of bytes to encode varNumber as a VAR-NUMBER in NDN-TLV.
 * @param varNumber The number to encode.
 * @return The number of bytes to encode varNumber.
 */
static __inline size_t
ndn_TlvEncoder_sizeOfVarNumber(uint64_t varNumber)
{
  if (varNumber < 253)
    return 1;
  else if (varNumber <= 0xffff)
    return 3;
  else if (varNumber <= 0xffffffff)
    return 5;
  else
    return 9;
}

/**
 * A private function to do the work of writeVarNumber, assuming that self->enableOutput is 1.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param varNumber The number to encode.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_TlvEncoder_writeVarNumberEnabled(struct ndn_TlvEncoder *self, uint64_t varNumber);

/**
 * Encode varNumber as a VAR-NUMBER in NDN-TLV and write it to self->output.  If self->enableOutput is 0,
 * then just advance self->offset without writing to output.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param varNumber The number to encode.
 * @return 0 for success, else an error code. If self->enableOutput is 0, this always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeVarNumber(struct ndn_TlvEncoder *self, uint64_t varNumber)
{
  if (self->enableOutput)
    return ndn_TlvEncoder_writeVarNumberEnabled(self, varNumber);
  else {
    // Just advance offset.
    self->offset += ndn_TlvEncoder_sizeOfVarNumber(varNumber);
    return NDN_ERROR_success;
  }
}

/**
 * Write the type and length to self->output.  If self->enableOutput is 0, then just advance self->offset without writing
 * to output.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param type the type of the TLV.
 * @param length The length of the TLV.
 * @return 0 for success, else an error code. If self->enableOutput is 0, this always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeTypeAndLength(struct ndn_TlvEncoder *self, unsigned int type, size_t length)
{
  if (self->enableOutput) {
    ndn_Error error;
    if ((error = ndn_TlvEncoder_writeVarNumberEnabled(self, (uint64_t)type)))
      return error;
    if ((error = ndn_TlvEncoder_writeVarNumberEnabled(self, (uint64_t)length)))
      return error;
  }
  else
    // Just advance offset.
    self->offset += ndn_TlvEncoder_sizeOfVarNumber((uint64_t)type) +
      ndn_TlvEncoder_sizeOfVarNumber((uint64_t)length);

  return NDN_ERROR_success;
}

/**
 * Return the number of bytes to encode value as a non-negative integer.
 * @param value The integer to encode.
 * @return The number of bytes to encode value.
 */
static __inline size_t
ndn_TlvEncoder_sizeOfNonNegativeInteger(uint64_t value)
{
  if (value <= 0xff)
    return 1;
  else if (value <= 0xffff)
    return 2;
  else if (value <= 0xffffffff)
    return 4;
  else
    return 8;
}

/**
 * A private function to do the work of writeNonNegativeInteger, assuming that self->enableOutput is 1.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param value The integer to encode.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_TlvEncoder_writeNonNegativeIntegerEnabled(struct ndn_TlvEncoder *self, uint64_t value);

/**
 * Encode value as a non-negative integer in NDN-TLV and write it to self->output.  If self->enableOutput is 0,
 * then just advance self->offset without writing to output.  This does not write a type or length for the value.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param value The integer to encode.
 * @return 0 for success, else an error code. If self->enableOutput is 0, this always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeNonNegativeInteger(struct ndn_TlvEncoder *self, uint64_t value)
{
  if (self->enableOutput)
    return ndn_TlvEncoder_writeNonNegativeIntegerEnabled(self, value);
  else {
    // Just advance offset.
    self->offset += ndn_TlvEncoder_sizeOfNonNegativeInteger(value);
    return NDN_ERROR_success;
  }
}

/**
 * Return the number of bytes to encode the type, length and blob value.
 * @param value A Blob with the array of bytes for the value.
 * @return The number of bytes to encode the TLV.
 */
static __inline size_t
ndn_TlvEncoder_sizeOfBlobTlv(unsigned int type, const struct ndn_Blob *value)
{
  return ndn_TlvEncoder_sizeOfVarNumber((uint64_t)type) + ndn_TlvEncoder_sizeOfVarNumber((uint64_t)value->length) +
    value->length;
}

/**
 * A private function to do the work of writeArray, assuming that
 * self->enableOutput is 1.
 * @param self pointer to the ndn_TlvEncoder struct.
 * @param array the array to copy.
 * @param arrayLength the length of the array.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_TlvEncoder_writeArrayEnabled
  (struct ndn_TlvEncoder *self, const uint8_t *array, size_t arrayLength);

/**
 * Copy the array to the output. Note that this does not encode a type and
 * length; for that see writeBlobTlv. If self->enableOutput is 0, then just
 * advance self->offset without writing to output.
 * @param self pointer to the ndn_TlvEncoder struct.
 * @param array the array to copy.
 * @param arrayLength the length of the array.
 * @return 0 for success, else an error code. If self->enableOutput is 0, this
 * always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeArray
  (struct ndn_TlvEncoder *self, const uint8_t *array, size_t arrayLength)
{
  if (self->enableOutput)
    return ndn_TlvEncoder_writeArrayEnabled(self, array, arrayLength);
  else
    // Just advance offset.
    self->offset += arrayLength;

  return NDN_ERROR_success;
}

/**
 * A private function to do the work of writeBlobTlv, assuming that self->enableOutput is 1.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param type the type of the TLV.
 * @param value A Blob with the array of bytes for the value.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_TlvEncoder_writeBlobTlvEnabled
  (struct ndn_TlvEncoder *self, unsigned int type, const struct ndn_Blob *value);

/**
 * Write the type, then the length of the blob then the blob value to self->output.  If self->enableOutput is 0,
 * then just advance self->offset without writing to output.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param type the type of the TLV.
 * @param value A Blob with the array of bytes for the value.
 * @return 0 for success, else an error code. If self->enableOutput is 0, this always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeBlobTlv
  (struct ndn_TlvEncoder *self, unsigned int type, const struct ndn_Blob *value)
{
  if (self->enableOutput)
    return ndn_TlvEncoder_writeBlobTlvEnabled(self, type, value);
  else
    // Just advance offset.
    self->offset += ndn_TlvEncoder_sizeOfBlobTlv(type, value);

  return NDN_ERROR_success;
}

/**
 * If value or valueLen is 0 then do nothing, otherwise call
 * ndn_TlvEncoder_writeOptionalBlobTlv.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param type the type of the TLV.
 * @param value A Blob with the array of bytes for the value.
 * @return 0 for success, else an error code. If self->enableOutput is 0, this always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeOptionalBlobTlv
  (struct ndn_TlvEncoder *self, unsigned int type, const struct ndn_Blob *value)
{
  if (value->value && value->length > 0)
    return ndn_TlvEncoder_writeBlobTlv(self, type, value);
  else
    return NDN_ERROR_success;
}

/**
 * Write the type, then the length of the encoded value then encode value as a non-negative integer
 * and write it to self->output.  If self->enableOutput is 0, then just advance self->offset without writing to output.
 * (If you want to just write the non-negative integer, use ndn_TlvEncoder_writeNonNegativeInteger.)
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param type the type of the TLV.
 * @param value The integer to encode.
 * @return 0 for success, else an error code. If self->enableOutput is 0, this always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeNonNegativeIntegerTlv(struct ndn_TlvEncoder *self, unsigned int type, uint64_t value)
{
  size_t sizeOfInteger = ndn_TlvEncoder_sizeOfNonNegativeInteger(value);
  if (self->enableOutput) {
    ndn_Error error;
    if ((error = ndn_TlvEncoder_writeTypeAndLength(self, type, sizeOfInteger)))
      return error;
    if ((error = ndn_TlvEncoder_writeNonNegativeIntegerEnabled(self, value)))
      return error;
  }
  else
    // Just advance offset.
    self->offset += ndn_TlvEncoder_sizeOfVarNumber((uint64_t)type) +
      ndn_TlvEncoder_sizeOfVarNumber((uint64_t)sizeOfInteger) + sizeOfInteger;

  return NDN_ERROR_success;
}

/**
 * If value is negative then do nothing, otherwise call ndn_TlvEncoder_writeNonNegativeIntegerTlv.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param type the type of the TLV.
 * @param value Negative for none, otherwise use (uint64_t)value.
 * @return 0 for success, else an error code. If self->enableOutput is 0, this always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv(struct ndn_TlvEncoder *self, unsigned int type, int value)
{
  if (value >= 0)
    return ndn_TlvEncoder_writeNonNegativeIntegerTlv(self, type, (uint64_t)value);
  else
    return NDN_ERROR_success;
}

/**
 * If value is negative then do nothing, otherwise round value to uint64_t and call ndn_TlvEncoder_writeNonNegativeIntegerTlv.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param type the type of the TLV.
 * @param value Negative for none, otherwise use (uint64_t)round(value).
 * @return 0 for success, else an error code. If self->enableOutput is 0, this always returns NDN_ERROR_success.
 */
static __inline ndn_Error
ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlvFromDouble(struct ndn_TlvEncoder *self, unsigned int type, double value)
{
  if (value >= 0.0)
    return ndn_TlvEncoder_writeNonNegativeIntegerTlv(self, type, (uint64_t)round(value));
  else
    return NDN_ERROR_success;
}

/**
 * Make a first pass to call writeValue with self->enableOutput = 0 to determine the length of the TLV. Then set
 * self->enableOutput = 1 and write the type and length to self->output and call writeValue again to write the
 * TLVs in the body of the value.  This is to solve the problem of finding the length when the value of a TLV has
 * nested TLVs.  However, if self->enableOutput is already 0 when this is called, then just advance self->offset without
 * writing to output.
 * @param self A pointer to the ndn_TlvEncoder struct.
 * @param type the type of the TLV.
 * @param writeValue A pointer to a function that writes the TLVs in the body of the value.  This calls
 * writeValue(context, self).
 * @param context A pointer to memory which is passed to writeValue.
 * @param omitZeroLength If 1 and the TLV length is zero, then don't write anything.
 * @return 0 for success, else an error code.
 */
ndn_Error
ndn_TlvEncoder_writeNestedTlv
  (struct ndn_TlvEncoder *self, unsigned int type,
   ndn_Error (*writeValue)(const void *context, struct ndn_TlvEncoder *encoder),
   const void *context, int omitZeroLength);

#ifdef  __cplusplus
}
#endif

#endif
