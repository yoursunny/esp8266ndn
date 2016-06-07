/**
 * Copyright (C) 2013-2016 Regents of the University of California.
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

#ifndef NDN_ELEMENT_LISTENER_H
#define NDN_ELEMENT_LISTENER_H

#include <ndn-cpp/c/common.h>
#include <ndn-cpp/c/encoding/element-reader-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_ElementListener struct to use the onReceivedElement
 * function pointer.
 * @param self pointer to the ndn_ElementListener struct
 * @param onReceivedElement When an entire packet element is received, call
 * onReceivedElement(self, element, elementLength) where self is the pointer to
 * this ndn_ElementListener struct, and element is a pointer to the array of
 * length elementLength with the bytes of the element.
 */
static __inline void ndn_ElementListener_initialize
  (struct ndn_ElementListener *self, ndn_OnReceivedElement onReceivedElement)
{
  self->onReceivedElement = onReceivedElement;
}

#ifdef __cplusplus
}
#endif

#endif
