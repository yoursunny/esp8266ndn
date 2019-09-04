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

#ifndef NDN_REGISTRATION_OPTIONS_TYPES_H
#define NDN_REGISTRATION_OPTIONS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An ndn_RegistrationOptions holds the options used when registering with the
 * forwarder to specify how to forward an interest and other options. We use a
 * separate ndn_RegistrationOptions to retain future compatibility if the
 * format of the registration command is changed.
 * (This was renamed from ndn_ForwardingFlags, which is deprecated.)
 */
struct ndn_RegistrationOptions {
  int childInherit;
  int capture;
  int origin; /**< -1 for none. */
};

#ifdef __cplusplus
}
#endif

#endif
