/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#ifndef NDN_DELEGATION_SET_TYPES_H
#define NDN_DELEGATION_SET_TYPES_H

#include "name-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_DelegationSet_Delegation holds a preference number and delegation name.
 */
struct ndn_DelegationSet_Delegation {
  int preference;
  struct ndn_Name name;
};

#ifdef __cplusplus
}
#endif

#endif
