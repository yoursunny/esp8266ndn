/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#ifndef NDN_ENCRYPT_PARAMS_TYPES_H
#define NDN_ENCRYPT_PARAMS_TYPES_H

#include "../../util/blob-types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  // These correspond to the TLV codes.
  ndn_EncryptAlgorithmType_AesEcb = 0,
  ndn_EncryptAlgorithmType_AesCbc = 1,
  ndn_EncryptAlgorithmType_RsaPkcs = 2,
  ndn_EncryptAlgorithmType_RsaOaep = 3
} ndn_EncryptAlgorithmType;

/**
 * An ndn_EncryptParams holds an algorithm type and other parameters used to
 * encrypt and decrypt.
 * @note This class is an experimental feature. The API may change.
 */
struct ndn_EncryptParams {
  ndn_EncryptAlgorithmType algorithmType; /**< -1 for none */
  struct ndn_Blob initialVector;
};

#ifdef __cplusplus
}
#endif

#endif
