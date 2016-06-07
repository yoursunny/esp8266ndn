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

#ifdef ARDUINO

#include "../../c/encoding/element-reader.h"
#include <ndn-cpp/lite/transport/arduino-yun-tcp-transport-lite.hpp>

namespace ndn {

ArduinoYunTcpTransportLite::ArduinoYunTcpTransportLite
  (DynamicUInt8ArrayLite& buffer)
{
  ndn_ElementReader_initialize(&elementReader_, 0, &buffer);
}

ndn_Error
ArduinoYunTcpTransportLite::connect
  (const char* host, int port, ElementListenerLite& elementListener)
{
  ndn_ElementReader_reset(&elementReader_, &elementListener);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  if (client_.connect(host, port) != 1)
    return NDN_ERROR_SocketTransport_cannot_connect_to_socket;

  return NDN_ERROR_success;
}

ndn_Error
ArduinoYunTcpTransportLite::send(const uint8_t* data, size_t dataLength)
{
  client_.write(data, dataLength);
  return NDN_ERROR_success;
}

ndn_Error
ArduinoYunTcpTransportLite::processEvents(uint8_t *buffer, size_t bufferLength)
{
  // Loop until there is no more data in the receive buffer.
  while(1) {
    if (!client_.available())
      return NDN_ERROR_success;

    size_t nBytes = client_.readBytes(buffer, bufferLength);
    if (nBytes == 0)
      return NDN_ERROR_success;

    ndn_Error error;
    if ((error = ndn_ElementReader_onReceivedData
         (&elementReader_, buffer, nBytes)))
      return error;
  }
}

ndn_Error
ArduinoYunTcpTransportLite::close()
{
  // For now, don't close.
  return NDN_ERROR_success;
}

}

#endif // ARDUINO
