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

#ifndef NDN_ARDUINO_YUN_TRANSPORT_LITE_HPP
#define NDN_ARDUINO_YUN_TRANSPORT_LITE_HPP

#ifdef ARDUINO

#include <YunClient.h>
#include "../../c/common.h"
#include "../../c/errors.h"
#include "../../c/encoding/element-reader-types.h"
#include "../util/dynamic-uint8-array-lite.hpp"
#include "../encoding/element-listener-lite.hpp"

namespace ndn {

/**
 * An ArduinoYunTransportLite uses the YunClient class of the Arduino Yun
 * bridge library to communicate over TCP.  (As of March 2015, the Yun bridge
 * library does not support UDP, so for now we provide this TCP support.)
 */
class ArduinoYunTcpTransportLite {
public:
  /**
   * Create a ArduinoYunTcpTransportLite with default values for no connection
   * yet and to use the given DynamicUInt8ArrayLite buffer for the
   * ElementReader. Note that the ElementReader is not valid until you call
   * connect.
   * @param buffer A DynamicUInt8ArrayLite which is used to save data before
   * calling the elementListener (see connect). The object must remain valid
   * during the entire life of this object. If the reallocFunction given to
   * buffer's constructor is 0, then its array must be large enough to save a
   * full Data or Interest packet.
   */
  ArduinoYunTcpTransportLite(DynamicUInt8ArrayLite& buffer);

  /**
   * Connect with TCP to the host:port. Note that your sketch must already have
   * called Bridge.begin().
   * @param host The host to connect to.
   * @param port The port to connect to.
   * @param elementListener The ElementListenerLite whose onReceivedElement
   * callback is called by processEvents. The elementListener must remain valid
   * during the life of this object or until replaced by the next call to
   * connect.
   * @return 0 for success, else an error code.
   */
  ndn_Error
  connect(const char* host, int port, ElementListenerLite& elementListener);

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

private:
  ndn_ElementReader elementReader_;
  YunClient client_;
};

}

#endif // ARDUINO

#endif
