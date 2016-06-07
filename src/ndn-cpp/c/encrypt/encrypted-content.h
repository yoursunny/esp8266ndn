/**
 * Copyright (C) 2016 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-group-encrypt src/encrypted-content https://github.com/named-data/ndn-group-encrypt
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

#ifndef NDN_ENCRYPTED_CONTENT_H
#define NDN_ENCRYPTED_CONTENT_H

#include <ndn-cpp/c/encrypt/encrypted-content-types.h>
#include "../key-locator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the ndn_EncryptedContent struct with values for none.
 * @param self A pointer to the ndn_EncryptedContent struct.
 * @param keyNameComponents The pre-allocated array of ndn_NameComponent for the
 * keyLocator.
 * @param maxKeyNameComponents The number of elements in the allocated
 * keyNameComponents array.
 */
static __inline void ndn_EncryptedContent_initialize
  (struct ndn_EncryptedContent *self, struct ndn_NameComponent *keyNameComponents,
   size_t maxKeyNameComponents) {
  self->algorithmType = (ndn_EncryptAlgorithmType)-1;
  ndn_KeyLocator_initialize
    (&self->keyLocator, keyNameComponents, maxKeyNameComponents);
  ndn_Blob_initialize(&self->initialVector, 0, 0);
  ndn_Blob_initialize(&self->payload, 0, 0);
}

/**
 * Set this ndn_EncryptedContent struct to have the values from the other
 * encryptedContent.
 * @param self A pointer to the ndn_EncryptedContent struct.
 * @param other A pointer to the other ndn_EncryptedContent to get values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's key locator keyName components array.
 */
static __inline ndn_Error
ndn_EncryptedContent_setFromEncryptedContent
  (struct ndn_EncryptedContent *self, const struct ndn_EncryptedContent *other)
{
  ndn_Error error;

  if (other == self)
    // Setting to itself. Do nothing.
    return NDN_ERROR_success;

  self->algorithmType = other->algorithmType;
  if ((error = ndn_KeyLocator_setFromKeyLocator
       (&self->keyLocator, &other->keyLocator)))
    return error;
  ndn_Blob_setFromBlob(&self->initialVector, &other->initialVector);
  ndn_Blob_setFromBlob(&self->payload, &other->payload);

  return NDN_ERROR_success;
}

#ifdef __cplusplus
}
#endif

#endif
