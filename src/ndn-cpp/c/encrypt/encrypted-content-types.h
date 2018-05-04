/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#ifndef NDN_ENCRYPTED_CONTENT_TYPES_H
#define NDN_ENCRYPTED_CONTENT_TYPES_H

#include "algo/encrypt-params-types.h"
#include "../key-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_EncryptedContent holds an encryption type, a payload and other fields
 * representing encrypted content.
 * @note This class is an experimental feature. The API may change.
 */
struct ndn_EncryptedContent {
  ndn_EncryptAlgorithmType algorithmType; /**< -1 for none */
  struct ndn_KeyLocator keyLocator;
  struct ndn_Blob initialVector;
  struct ndn_Blob payload;
};

#ifdef __cplusplus
}
#endif

#endif
