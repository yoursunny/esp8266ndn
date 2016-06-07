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

#ifndef NDN_TCP_TRANSPORT_LITE_HPP
#define NDN_TCP_TRANSPORT_LITE_HPP

#include "../../c/errors.h"
#include "../../c/transport/transport-types.h"
#include "../util/dynamic-uint8-array-lite.hpp"
#include "../encoding/element-listener-lite.hpp"

namespace ndn {

class TcpTransportLite : private ndn_TcpTransport {
public:
  /**
   * Create a TcpTransport with default values for no connection yet and to use
   * the given DynamicUInt8ArrayLite buffer for the ElementReader. Note that the
   * ElementReader is not valid until you call connect.
   * @param buffer A DynamicUInt8ArrayLite which is used to save data before
   * calling the elementListener (see connect). The object must remain valid
   * during the entire life of this object. If the reallocFunction given to
   * buffer's constructor is 0, then its array must be large enough to save a
   * full element, perhaps MAX_NDN_PACKET_SIZE bytes.
   */
  TcpTransportLite(DynamicUInt8ArrayLite& buffer);

  /**
   * Determine whether this transport connecting to the host is
   * to a node on the current machine; results are not cached. According to
   * http://redmine.named-data.net/projects/nfd/wiki/ScopeControl#local-face,
   * TCP transports with a loopback address are local. If host is a host name,
   * this will do a blocking DNS lookup; otherwise this will parse the IP address
   * and examine the first octet to determine if it is a loopback address (e.g.
   * the first IPv4 octet is 127 or IPv6 is "::1").
   * @param host The host to check.
   * @param result Set result to true if the host is local, false if not.
   * @return 0 for success, else an error code.
   */
  ndn_Error
  isLocal(const char *host, bool& result);

  /**
   * Connect with TCP to the host:port.
   * @param host The host to connect to.
   * @param port The port to connect to.
   * @param elementListener The ElementListenerLite whose onReceivedElement
   * callback is called by processEvents. The elementListener must remain valid
   * during the life of this object or until replaced by the next call to
   * connect.
   * @return 0 for success, else an error code.
   */
  ndn_Error
  connect(const char* host, unsigned short port, ElementListenerLite& elementListener);

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
   * Downcast the reference to the ndn_TcpTransport struct to a TcpTransportLite.
   * @param transport A reference to the ndn_TcpTransport struct.
   * @return The same reference as TcpTransportLite.
   */
  static TcpTransportLite&
  downCast(ndn_TcpTransport& transport) { return *(TcpTransportLite*)&transport; }

  static const TcpTransportLite&
  downCast(const ndn_TcpTransport& transport) { return *(TcpTransportLite*)&transport; }
};

}

#endif
