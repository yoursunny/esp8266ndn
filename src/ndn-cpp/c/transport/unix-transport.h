/*
 * Copyright (C) 2014-2016 Regents of the University of California.
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

#ifndef NDN_UNIX_TRANSPORT_H
#define NDN_UNIX_TRANSPORT_H

#include "socket-transport.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the ndn_UnixTransport struct with default values for no connection
 * yet and to use the given buffer for the ElementReader. Note that
 * the ElementReader is not valid until you call ndn_UnixTransport_connect.
 * @param self A pointer to the ndn_UnixTransport struct.
 * @param buffer A pointer to a ndn_DynamicUInt8Array struct which is used to
 * save data before calling the elementListener (see ndn_SocketTransport_connect).
 * The struct must remain valid during the entire life of this
 * ndn_SocketTransport. If the buffer->realloc function pointer is 0, its array
 * must be large enough to save a full element, perhaps MAX_NDN_PACKET_SIZE bytes.
 */
static __inline void ndn_UnixTransport_initialize
  (struct ndn_UnixTransport *self, struct ndn_DynamicUInt8Array *buffer)
{
  ndn_SocketTransport_initialize(&self->base, buffer);
}

/**
 * Determine whether this transport is to a node on the current machine.
 * Unix transports are always local.
 * @return 1 because Unix transports are always local.
 */
static __inline int
ndn_UnixTransport_isLocal() { return 1; }

/**
 * Connect with a Unix Socket to the socket filePath.
 * @param self A pointer to the ndn_UnixTransport struct.
 * @param filePath The file path of the Unix socket to connect to.
 * @param elementListener A pointer to the ndn_ElementListener used by
 * ndn_SocketTransport_processEvents, which remain valid during the life of this
 * object or until replaced by the next call to connect.
 * @return 0 for success, else an error code.
 */
static __inline ndn_Error ndn_UnixTransport_connect
  (struct ndn_UnixTransport *self, char *filePath,
   struct ndn_ElementListener *elementListener)
{
  return ndn_SocketTransport_connect
    (&self->base, SOCKET_UNIX, filePath, 0, elementListener);
}

/**
 * Send data to the socket.
 * @param self A pointer to the ndn_UnixTransport struct.
 * @param data A pointer to the buffer of data to send.
 * @param dataLength The number of bytes in data.
 * @return 0 for success, else an error code.
 */
static __inline ndn_Error ndn_UnixTransport_send
  (struct ndn_UnixTransport *self, const uint8_t *data, size_t dataLength)
{
  return ndn_SocketTransport_send(&self->base, data, dataLength);
}

/**
 * Process any data to receive.  For each element received, call
 * (*elementListener->onReceivedElement)(element, elementLength) for the
 * elementListener in the elementReader given to connect(). This is non-blocking
 * and will return immediately if there is no data to receive.
 * @param self A pointer to the ndn_UnixTransport struct.
 * @param buffer A pointer to a buffer for receiving data. Note that this is
 * only for temporary use and is not the way that this function supplies data.
 * It supplies the data by calling the onReceivedElement callback.
 * @param bufferLength The size of buffer. The buffer should be as large as
 * resources permit up to MAX_NDN_PACKET_SIZE, but smaller sizes will work
 * however may be less efficient due to multiple calls to socket receive and
 * more processing by the ElementReader.
 * @return 0 for success, else an error code.
 */
static __inline ndn_Error
ndn_UnixTransport_processEvents
  (struct ndn_UnixTransport *self, uint8_t *buffer, size_t bufferLength)
{
  return ndn_SocketTransport_processEvents(&self->base, buffer, bufferLength);
}

/**
 * Close the socket.
 * @param self A pointer to the ndn_UnixTransport struct.
 * @return 0 for success, else an error code.
 */
static __inline ndn_Error ndn_UnixTransport_close(struct ndn_UnixTransport *self)
{
  return ndn_SocketTransport_close(&self->base);
}

#ifdef __cplusplus
}
#endif

#endif
