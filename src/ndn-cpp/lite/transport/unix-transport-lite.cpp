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

#include "../../c/transport/unix-transport.h"
#include <ndn-cpp/lite/transport/unix-transport-lite.hpp>

namespace ndn {

UnixTransportLite::UnixTransportLite(DynamicUInt8ArrayLite& buffer)
{
  ndn_UnixTransport_initialize(this, &buffer);
}

bool
UnixTransportLite::isLocal() { return ndn_UnixTransport_isLocal() != 0; }

ndn_Error
UnixTransportLite::connect(char* filePath, ElementListenerLite& elementListener)
{
  return ndn_UnixTransport_connect(this, filePath, &elementListener);
}

ndn_Error
UnixTransportLite::send(const uint8_t* data, size_t dataLength)
{
  return ndn_UnixTransport_send(this, data, dataLength);
}

ndn_Error
UnixTransportLite::processEvents(uint8_t *buffer, size_t bufferLength)
{
  return ndn_UnixTransport_processEvents(this, buffer, bufferLength);
}

ndn_Error
UnixTransportLite::close()
{
  return ndn_UnixTransport_close(this);
}

}

#endif // NDN_CPP_HAVE_UNISTD_H
