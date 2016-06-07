/**
 * Copyright (C) 2016 Regents of the University of California.
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

#ifndef NDN_DELEGATION_SET_H
#define NDN_DELEGATION_SET_H

#include <ndn-cpp/c/delegation-set-types.h>
#include "name.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_DelegationSet_Delegation struct with the pre-allocated
 * nameComponents, and defaults for all the values.
 * @param self A pointer to the ndn_DelegationSet_Delegation struct.
 * @param nameComponents The pre-allocated array of ndn_NameComponent.
 * @param maxNameComponents The number of elements in the allocated
 * nameComponents array.
 */
static __inline void ndn_DelegationSet_Delegation_initialize
  (struct ndn_DelegationSet_Delegation *self,
   struct ndn_NameComponent *nameComponents, size_t maxNameComponents)
{
  self->preference = 0;
  ndn_Name_initialize(&self->name, nameComponents, maxNameComponents);
}

/**
 * Set this ndn_DelegationSet_Delegation struct to have the values from the
 * other delegation.
 * @param self A pointer to the ndn_DelegationSet_Delegation struct.
 * @param other A pointer to the other ndn_DelegationSet_Delegation to get
 * values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's name components array.
 */
static __inline ndn_Error
ndn_DelegationSet_Delegation_setFromDelegationSet_Delegation
  (struct ndn_DelegationSet_Delegation *self,
   const struct ndn_DelegationSet_Delegation *other)
{
  ndn_Error error;

  if (other == self)
    // Setting to itself. Do nothing.
    return NDN_ERROR_success;

  self->preference = other->preference;
  if ((error = ndn_Name_setFromName(&self->name, &other->name)))
    return error;

  return NDN_ERROR_success;
}

#ifdef __cplusplus
}
#endif

#endif
