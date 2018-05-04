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

#ifndef NDN_INTEREST_TYPES_H
#define NDN_INTEREST_TYPES_H

#include "name-types.h"
#include "key-types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ndn_Exclude_COMPONENT = 0,
  ndn_Exclude_ANY = 1
} ndn_ExcludeType;

/**
 * An ndn_ExcludeEntry holds an ndn_ExcludeType, and if it is a COMPONENT, it holds a pointer to the component value.
 */
struct ndn_ExcludeEntry {
  ndn_ExcludeType type;
  struct ndn_NameComponent component;
};

/**
 * An ndn_Exclude holds an array of ndn_ExcludeEntry.
 */
struct ndn_Exclude {
  struct ndn_ExcludeEntry *entries;  /**< pointer to the array of entries. */
  size_t maxEntries;                 /**< the number of elements in the allocated entries array */
  size_t nEntries;                   /**< the number of entries in the exclude, 0 for no exclude */
};

enum {
  ndn_Interest_CHILD_SELECTOR_LEFT = 0,
  ndn_Interest_CHILD_SELECTOR_RIGHT = 1
};

/**
 * An ndn_Interest holds an ndn_Name and other fields for an interest.
 */
struct ndn_Interest {
  struct ndn_Name name;
  int minSuffixComponents;  /**< -1 for none */
  int maxSuffixComponents;  /**< -1 for none */
  struct ndn_KeyLocator keyLocator;
  struct ndn_Exclude exclude;
  int childSelector;        /**< -1 for none */
  int mustBeFresh;          /**< bool. Default true. */
  ndn_Milliseconds interestLifetimeMilliseconds; /**< -1.0 for none */
  struct ndn_Blob nonce;    /**< The blob whose value is a pointer to a pre-allocated buffer.  0 for none */
  struct ndn_Blob forwardingHintWireEncoding; /**< The blob whose value is a pointer to a pre-allocated buffer
                                               of the encoded sequence of delegation set (with no outer TLV).  0 for none */
  struct ndn_Blob linkWireEncoding; /**< The link whose value is a pointer to a pre-allocated buffer.  0 for none */
  int selectedDelegationIndex; /**< -1 for none */
};

#ifdef __cplusplus
}
#endif

#endif
