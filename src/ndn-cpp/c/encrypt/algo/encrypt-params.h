/**
 * Copyright (C) 2016 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-group-encrypt src/encrypt-params https://github.com/named-data/ndn-group-encrypt
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

#ifndef NDN_ENCRYPT_PARAMS_H
#define NDN_ENCRYPT_PARAMS_H

#include <ndn-cpp/c/encrypt/algo/encrypt-params-types.h>
#include "../../util/blob.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_EncryptParams struct with defaults for all the values.
 * @param self A pointer to the ndn_EncryptParams struct.
 */
static __inline void ndn_EncryptParams_initialize(struct ndn_EncryptParams *self)
{
  self->algorithmType = (ndn_EncryptAlgorithmType)-1;
  ndn_Blob_initialize(&self->initialVector, 0, 0);
}

#ifdef __cplusplus
}
#endif

#endif
