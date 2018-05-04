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

#include "../../util/endian.h"
#include "tlv-decoder.h"

ndn_Error
ndn_TlvDecoder_readExtendedVarNumber(struct ndn_TlvDecoder *self, unsigned int firstOctet, uint64_t *varNumber)
{
  // This is a private function so we know firstOctet >= 253.
  if (firstOctet == 253) {
    uint16_t beValue;
    if (self->offset + 2 > self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;

#if NDN_CPP_HAVE_ENDIAN_H
    // kind of dangerous... but should be efficient.
    beValue = *(uint16_t *)(self->input + self->offset);
    *varNumber = (uint64_t)be16toh(beValue);
#else
    *varNumber = (((uint16_t)self->input[self->offset]) << 8) +
                  ((uint16_t)self->input[self->offset + 1]);
#endif
    self->offset += 2;
  }
  else if (firstOctet == 254) {
    uint32_t beValue;
    if (self->offset + 4 > self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;

#if NDN_CPP_HAVE_ENDIAN_H
    // kind of dangerous... but should be efficient.
    beValue = *(uint32_t *)(self->input + self->offset);
    *varNumber = (uint64_t)be32toh(beValue);
#else
    *varNumber = (((uint32_t)self->input[self->offset]) << 24) +
                 (((uint32_t)self->input[self->offset + 1]) << 16) +
                 (((uint32_t)self->input[self->offset + 2]) << 8) +
                  ((uint32_t)self->input[self->offset + 3]);
#endif
    self->offset += 4;
  }
  else {
    // value == 255.
    uint64_t beValue;
    if (self->offset + 8 > self->inputLength)
      return NDN_ERROR_read_past_the_end_of_the_input;

#if NDN_CPP_HAVE_ENDIAN_H
    // kind of dangerous... but should be efficient.
    beValue = *(uint64_t *)(self->input + self->offset);
    *varNumber = be64toh(beValue);
#else
    *varNumber = (((uint64_t)self->input[self->offset]) << 56) +
                 (((uint64_t)self->input[self->offset + 1]) << 48) +
                 (((uint64_t)self->input[self->offset + 2]) << 40) +
                 (((uint64_t)self->input[self->offset + 3]) << 32) +
                 (((uint64_t)self->input[self->offset + 4]) << 24) +
                 (((uint64_t)self->input[self->offset + 5]) << 16) +
                 (((uint64_t)self->input[self->offset + 6]) << 8) +
                  ((uint64_t)self->input[self->offset + 7]);
#endif
    self->offset += 8;
  }

  return NDN_ERROR_success;
}

ndn_Error
ndn_TlvDecoder_skipRemainingNestedTlvs
  (struct ndn_TlvDecoder *self, size_t endOffset)
{
  while(self->offset < endOffset) {
    ndn_Error error;
    uint64_t dummyType;
    uint64_t lengthVarNumber;

    if ((error = ndn_TlvDecoder_readVarNumber(self, &dummyType)))
      return error;

    if ((error = ndn_TlvDecoder_readVarNumber(self, &lengthVarNumber)))
      return error;
    // Silently ignore if the length is larger than size_t.
    self->offset += (size_t)lengthVarNumber;
    if (self->offset > self->inputLength)
      return NDN_ERROR_TLV_length_exceeds_buffer_length;
  }

  if (self->offset != endOffset)
    return NDN_ERROR_TLV_length_does_not_equal_total_length_of_nested_TLVs;

  return NDN_ERROR_success;
}

ndn_Error
ndn_TlvDecoder_readExtendedNonNegativeInteger(struct ndn_TlvDecoder *self, size_t length, uint64_t *value)
{
  // This is a private function so we know length != 1.
  switch (length) {
    case 2:
      if (self->offset + 2 > self->inputLength)
        return NDN_ERROR_read_past_the_end_of_the_input;

#if NDN_CPP_HAVE_ENDIAN_H
      {
        // kind of dangerous... but should be efficient.
        uint16_t beValue = *(uint16_t *)(self->input + self->offset);
        *value = (uint64_t)be16toh(beValue);
      }
#else
      *value = (((uint16_t)self->input[self->offset]) << 8) +
                ((uint16_t)self->input[self->offset + 1]);
#endif
        self->offset += 2;
      return NDN_ERROR_success;
    case 4:
      if (self->offset + 4 > self->inputLength)
        return NDN_ERROR_read_past_the_end_of_the_input;

#if NDN_CPP_HAVE_ENDIAN_H
      {
        // kind of dangerous... but should be efficient.
        uint32_t beValue = *(uint32_t *)(self->input + self->offset);
        *value = (uint64_t)be32toh(beValue);
      }
#else
      *value = (((uint32_t)self->input[self->offset]) << 24) +
               (((uint32_t)self->input[self->offset + 1]) << 16) +
               (((uint32_t)self->input[self->offset + 2]) << 8) +
                ((uint32_t)self->input[self->offset + 3]);
#endif
      self->offset += 4;
      return NDN_ERROR_success;
    case 8:
      if (self->offset + 8 > self->inputLength)
        return NDN_ERROR_read_past_the_end_of_the_input;

#if NDN_CPP_HAVE_ENDIAN_H
      {
        // kind of dangerous... but should be efficient.
        uint64_t beValue = *(uint64_t *)(self->input + self->offset);
        *value = be64toh(beValue);
      }
#else
      *value = (((uint64_t)self->input[self->offset]) << 56) +
               (((uint64_t)self->input[self->offset + 1]) << 48) +
               (((uint64_t)self->input[self->offset + 2]) << 40) +
               (((uint64_t)self->input[self->offset + 3]) << 32) +
               (((uint64_t)self->input[self->offset + 4]) << 24) +
               (((uint64_t)self->input[self->offset + 5]) << 16) +
               (((uint64_t)self->input[self->offset + 6]) << 8) +
                ((uint64_t)self->input[self->offset + 7]);
#endif
      self->offset += 8;
      return NDN_ERROR_success;
  }

  return NDN_ERROR_Invalid_length_for_nonNegativeInteger;
}
