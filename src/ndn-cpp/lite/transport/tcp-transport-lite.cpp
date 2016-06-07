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

// Only compile if we have Unix socket support.
#include <ndn-cpp/ndn-cpp-config.h>
#if NDN_CPP_HAVE_UNISTD_H

#include "../../c/transport/tcp-transport.h"
#include <ndn-cpp/lite/transport/tcp-transport-lite.hpp>

namespace ndn {

TcpTransportLite::TcpTransportLite(DynamicUInt8ArrayLite& buffer)
{
  ndn_TcpTransport_initialize(this, &buffer);
}

ndn_Error
TcpTransportLite::isLocal(const char *host, bool& result)
{
  int intResult;
  ndn_Error status = ndn_TcpTransport_isLocal(host, &intResult);
  result = (intResult != 0);

  return status;
}

ndn_Error
TcpTransportLite::connect
  (const char* host, unsigned short port, ElementListenerLite& elementListener)
{
  return ndn_TcpTransport_connect(this, host, port, &elementListener);
}

ndn_Error
TcpTransportLite::send(const uint8_t* data, size_t dataLength)
{
  return ndn_TcpTransport_send(this, data, dataLength);
}

ndn_Error
TcpTransportLite::processEvents(uint8_t *buffer, size_t bufferLength)
{
  return ndn_TcpTransport_processEvents(this, buffer, bufferLength);
}

ndn_Error
TcpTransportLite::close() { return ndn_TcpTransport_close(this); }

}

#endif // NDN_CPP_HAVE_UNISTD_H
