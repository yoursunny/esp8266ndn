/**
 * Copyright (C) 2015-2016 Regents of the University of California.
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

/* Note: This file should be named forward-flags.h (like data.h) but
 * we make it forwarding-flags-impl.h because forwarding-flags.h is already in
 * the public include directory for backwards compatibility reasons.
 */

#ifndef NDN_FORWARDING_FLAGS_IMPL_H
#define NDN_FORWARDING_FLAGS_IMPL_H

// Normally, this include would have been called forwarding-flags-types.h .
#include <ndn-cpp/c/forwarding-flags.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_ForwardingFlags struct with the default with "childInherit"
 * set and all other flags cleared.
 * @param self A pointer to the ndn_ForwardingFlags struct.
 */
void ndn_ForwardingFlags_initialize(struct ndn_ForwardingFlags *self);

/**
 * Get an integer with the bits set according to the NFD forwarding flags as
 * used in the ControlParameters of the command interest.
 * @param self A pointer to the ndn_ForwardingFlags struct.
 * @return An integer with the bits set.
 */
int ndn_ForwardingFlags_getNfdForwardingFlags(const struct ndn_ForwardingFlags *self);

/**
 * Set the flags according to the NFD forwarding flags as used in the
 * ControlParameters of the command interest.
 * @param self A pointer to the ndn_ForwardingFlags struct.
 * @param nfdForwardingFlags An integer with the bits set.
 */
void ndn_ForwardingFlags_setNfdForwardingFlags
  (struct ndn_ForwardingFlags *self, int nfdForwardingFlags);

#ifdef __cplusplus
}
#endif

#endif
