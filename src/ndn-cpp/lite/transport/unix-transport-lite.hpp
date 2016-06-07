/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
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

#ifndef NDN_UNIX_TRANSPORT_LITE_HPP
#define NDN_UNIX_TRANSPORT_LITE_HPP

#include "../../c/errors.h"
#include "../../c/transport/transport-types.h"
#include "../util/dynamic-uint8-array-lite.hpp"
#include "../encoding/element-listener-lite.hpp"

namespace ndn {

class UnixTransportLite : private ndn_UnixTransport {
public:
  /**
   * Create a UnixTransport with default values for no connection yet and to use
   * the given DynamicUInt8ArrayLite buffer for the ElementReader. Note that the
   * ElementReader is not valid until you call connect.
   * @param buffer A DynamicUInt8ArrayLite which is used to save data before
   * calling the elementListener (see connect). The object must remain valid
   * during the entire life of this object. If the reallocFunction given to
   * buffer's constructor is 0, then its array must be large enough to save a
   * full element, perhaps MAX_NDN_PACKET_SIZE bytes.
   */
  UnixTransportLite(DynamicUInt8ArrayLite& buffer);

  /**
   * Determine whether this transport is to a node on the current machine.
   * Unix transports are always local.
   * @return True because Unix transports are always local.
   */
  static bool
  isLocal();

  /**
   * Connect with a Unix Socket to the socket filePath.
   * @param filePath The file path of the Unix socket to connect to.
   * @param elementListener The ElementListenerLite whose onReceivedElement
   * callback is called by processEvents. The elementListener must remain valid
   * during the life of this object or until replaced by the next call to
   * connect.
   * @return 0 for success, else an error code.
   */
  ndn_Error
  connect(char* filePath, ElementListenerLite& elementListener);

  /**
   * Send data to the socket.
   * @param data A pointer to the buffer of data to send.
   * @param dataLength The number of bytes in data.
   * @return 0 for success, else an error code.
   */
  ndn_Error
  send(const uint8_t* data, size_t dataLength);

  /**
   * Process any data to receive.  For each element received, call
   * (*elementListener->onReceivedElement)(element, elementLength) for the
   * elementListener in the elementReader given to connect(). This is non-blocking
   * and will return immediately if there is no data to receive.
   * @param buffer A pointer to a buffer for receiving data. Note that this is
   * only for temporary use and is not the way that this function supplies data.
   * It supplies the data by calling the onReceivedElement callback.
   * @param bufferLength The size of buffer. The buffer should be as large as
   * resources permit up to MAX_NDN_PACKET_SIZE, but smaller sizes will work
   * however may be less efficient due to multiple calls to socket receive and
   * more processing by the ElementReader.
   * @return 0 for success, else an error code.
   */
  ndn_Error
  processEvents(uint8_t *buffer, size_t bufferLength);

  /**
   * Close the socket.
   * @return 0 for success, else an error code.
   */
  ndn_Error
  close();

  /**
   * Downcast the reference to the ndn_UnixTransport struct to a UnixTransportLite.
   * @param transport A reference to the ndn_UnixTransport struct.
   * @return The same reference as UnixTransportLite.
   */
  static UnixTransportLite&
  downCast(ndn_UnixTransport& transport) { return *(UnixTransportLite*)&transport; }

  static const UnixTransportLite&
  downCast(const ndn_UnixTransport& transport) { return *(UnixTransportLite*)&transport; }
};

}

#endif
