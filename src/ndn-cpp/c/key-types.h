/**
 * Copyright (C) 2013-2018 Regents of the University of California.
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

#ifndef NDN_KEY_TYPES_H
#define NDN_KEY_TYPES_H

#include "name-types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_KeyLocatorType_KEYNAME = 1,
  ndn_KeyLocatorType_KEY_LOCATOR_DIGEST = 2
} ndn_KeyLocatorType;

/**
 * An ndn_KeyLocator holds the type of key locator and related data.
 */
struct ndn_KeyLocator {
  ndn_KeyLocatorType type;     /**< -1 for none */
  struct ndn_Blob keyData;            /**< A Blob whose value is a pointer to a pre-allocated buffer for the key data as follows:
    * If type is ndn_KeyLocatorType_KEY_LOCATOR_DIGEST, the digest data.
    */
  struct ndn_Name keyName;     /**< The key name (only used if type is ndn_KeyLocatorType_KEYNAME.) */
};

#ifdef __cplusplus
}
#endif

#endif
