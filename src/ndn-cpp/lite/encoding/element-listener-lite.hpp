/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2015-2018 Regents of the University of California.
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

#ifndef NDN_ELEMENT_LISTENER_LITE_HPP
#define NDN_ELEMENT_LISTENER_LITE_HPP

#include "../../c/encoding/element-reader-types.h"

namespace ndn {

class ElementListenerLite;
typedef void (*OnReceivedElementLite)
  (ElementListenerLite *self, const uint8_t *element, size_t elementLength);

/** An ElementListenerLite holds an OnReceivedElementLite function pointer.
 * You can use this class as is, or extend it to provide data that can be
 * accessed through the self pointer in onReceivedElement.
 */
class ElementListenerLite : private ndn_ElementListener {
public:
  /**
   * Create an ElementListenerLite to use the onReceivedElement function pointer.
   * @param onReceivedElement When an entire packet element is received, call
   * onReceivedElement(ElementListenerLite *self, uint8_t *element, size_t elementLength)
   * where self is the pointer to this object, and element is a pointer to the
   * array of length elementLength with the bytes of the element. If you
   * created a derived class, you can downcast self to a pointer to your derived
   * class in order to access its members.
   */
  ElementListenerLite(OnReceivedElementLite onReceivedElement);

private:
  // Declare friends who can downcast to the private base.
  friend class TcpTransportLite;
  friend class UdpTransportLite;
  friend class UnixTransportLite;
  friend class ArduinoYunTcpTransportLite;
  friend class Esp8266TcpTransportLite;
};

}

#endif
