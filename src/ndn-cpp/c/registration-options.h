/**
 * Copyright (C) 2019 Regents of the University of California.
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

#ifndef NDN_REGISTRATION_OPTIONS_H
#define NDN_REGISTRATION_OPTIONS_H

#include "../c/registration-options-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_RegistrationOptions struct with the default with "childInherit"
 * set and all other flags cleared.
 * @param self A pointer to the ndn_RegistrationOptions struct.
 */
void
ndn_RegistrationOptions_initialize(struct ndn_RegistrationOptions *self);

/**
 * Get an integer with the bits set according to the NFD forwarding flags as
 * used in the ControlParameters of the command interest.
 * @param self A pointer to the ndn_RegistrationOptions struct.
 * @return An integer with the bits set.
 */
int
ndn_RegistrationOptions_getNfdForwardingFlags
  (const struct ndn_RegistrationOptions *self);

/**
 * Set the flags according to the NFD forwarding flags as used in the
 * ControlParameters of the command interest.
 * This ignores the origin value.
 * @param self A pointer to the ndn_RegistrationOptions struct.
 * @param nfdForwardingFlags An integer with the bits set.
 */
void
ndn_RegistrationOptions_setNfdForwardingFlags
  (struct ndn_RegistrationOptions *self, int nfdForwardingFlags);

#ifdef __cplusplus
}
#endif

#endif
