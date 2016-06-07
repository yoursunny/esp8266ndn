/**
 * Copyright (C) 2016 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-cxx packet.hpp https://github.com/named-data/ndn-cxx/blob/master/src/lp/packet.hpp
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

#ifndef NDN_LP_PACKET_H
#define NDN_LP_PACKET_H

#include "../util/blob.h"
#include <ndn-cpp/c/errors.h>
#include <ndn-cpp/c/lp/lp-packet-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_LpPacket struct with the headerFields array.
 * @param self A pointer to the ndn_LpPacket struct.
 * @param headerFields The pre-allocated array of ndn_LpPacketHeaderField.
 * @param maxHeaderFields The number of elements in the allocated headerFields
 * array.
 */
static __inline void
ndn_LpPacket_initialize
  (struct ndn_LpPacket *self, struct ndn_LpPacketHeaderField *headerFields,
   size_t maxHeaderFields)
{
  self->headerFields = headerFields;
  self->maxHeaderFields = maxHeaderFields;
  self->nHeaderFields = 0;
  ndn_Blob_initialize(&self->fragmentWireEncoding, 0, 0);
}

/**
 * Clear all header fields and set the fragment to an isNull Blob.
 * @param self A pointer to the ndn_LpPacket struct.
 */
static __inline void
ndn_LpPacket_clear(struct ndn_LpPacket *self)
{ 
  self->nHeaderFields = 0;
  ndn_Blob_initialize(&self->fragmentWireEncoding, 0, 0);
}

/**
 * Add an uninitialized header field. You must set the type and initialize it.
 * @param self A pointer to the ndn_LpPacket struct.
 * @param headerField Set headerField to a pointer to the newly-added
 * ndn_LpPacketHeaderField struct.
 * @return 0 for success, or an error code if there is no more room in the
 * fieldHeaders array (nHeaderFields is already maxHeaderFields).
 */
static __inline ndn_Error
ndn_LpPacket_addEmptyHeaderField
  (struct ndn_LpPacket *self, struct ndn_LpPacketHeaderField **headerField)
{
  if (self->nHeaderFields >= self->maxHeaderFields)
    return NDN_ERROR_cannot_add_an_entry_past_the_maximum_number_of_entries_allowed_in_the_LpPacket;
  *headerField = self->headerFields + self->nHeaderFields;
  ++self->nHeaderFields;

  return NDN_ERROR_success;
}

#ifdef __cplusplus
}
#endif

#endif
