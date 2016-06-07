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

// Only compile if we have Unix socket support.
#include <ndn-cpp/ndn-cpp-config.h>
#if NDN_CPP_HAVE_UNISTD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <poll.h>
#include "../util/ndn_memory.h"
#include "socket-transport.h"
#include <errno.h>

ndn_Error ndn_SocketTransport_connect
  (struct ndn_SocketTransport *self, ndn_SocketType socketType, const char *host,
   unsigned short port, struct ndn_ElementListener *elementListener)
{
  int socketDescriptor;

  ndn_ElementReader_reset(&self->elementReader, elementListener);

  if (socketType == SOCKET_UNIX) {
    struct sockaddr_un address;
    memset(&address, 0, sizeof(struct sockaddr_un));

    if ((socketDescriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
      return NDN_ERROR_SocketTransport_cannot_connect_to_socket;

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, host);
    if (connect(socketDescriptor, (struct sockaddr *)&address,
                SUN_LEN(&address)) == -1) {
      close(socketDescriptor);
      return NDN_ERROR_SocketTransport_cannot_connect_to_socket;
    }
  }
  else {
    struct addrinfo hints;
    char portString[10];
    struct addrinfo *serverInfo;
    struct addrinfo *p;

    if (self->socketDescriptor >= 0) {
      close(self->socketDescriptor);
      self->socketDescriptor = -1;
    }

    ndn_memset((uint8_t *)&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    if (socketType == SOCKET_TCP)
      hints.ai_socktype = SOCK_STREAM;
    else if (socketType == SOCKET_UDP)
      hints.ai_socktype = SOCK_DGRAM;
    else
      return NDN_ERROR_unrecognized_ndn_SocketTransport;

    sprintf(portString, "%d", port);

    if (getaddrinfo(host, portString, &hints, &serverInfo) != 0)
      return NDN_ERROR_SocketTransport_error_in_getaddrinfo;

    // loop through all the results and connect to the first we can
    for(p = serverInfo; p != NULL; p = p->ai_next) {
      if ((socketDescriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        continue;

      if (connect(socketDescriptor, p->ai_addr, p->ai_addrlen) == -1) {
        close(socketDescriptor);
        continue;
      }

      break;
    }

    if (p == NULL) {
      freeaddrinfo(serverInfo);
      return NDN_ERROR_SocketTransport_cannot_connect_to_socket;
    }

    freeaddrinfo(serverInfo);
  }

  self->socketDescriptor = socketDescriptor;
  return NDN_ERROR_success;
}

ndn_Error ndn_SocketTransport_send(struct ndn_SocketTransport *self, const uint8_t *data, size_t dataLength)
{
  if (self->socketDescriptor < 0)
    return NDN_ERROR_SocketTransport_socket_is_not_open;

  int nBytes;
  while (1) {
    if ((nBytes = send(self->socketDescriptor, data, dataLength, 0)) < 0)
      return NDN_ERROR_SocketTransport_error_in_send;
    if (nBytes >= dataLength)
      break;

    // Send more.
    dataLength -= nBytes;
  }

  return NDN_ERROR_success;
}

ndn_Error ndn_SocketTransport_receiveIsReady(struct ndn_SocketTransport *self, int *receiveIsReady)
{
  // Default to not ready.
  *receiveIsReady = 0;

  if (self->socketDescriptor < 0)
    // The socket is not open.  Just silently return.
    return NDN_ERROR_success;

  struct pollfd pollInfo[1];
  pollInfo[0].fd = self->socketDescriptor;
  pollInfo[0].events = POLLIN;

  int pollResult = poll(pollInfo, 1, 0);

  if (pollResult < 0)
    return NDN_ERROR_SocketTransport_error_in_poll;
  else if (pollResult == 0)
    // Timeout, so no data ready.
    return NDN_ERROR_success;
  else {
   if (pollInfo[0].revents & POLLIN)
     *receiveIsReady = 1;
  }

  return NDN_ERROR_success;
}

ndn_Error ndn_SocketTransport_receive
  (struct ndn_SocketTransport *self, uint8_t *buffer, size_t bufferLength, size_t *nBytesOut)
{
  if (self->socketDescriptor < 0)
    return NDN_ERROR_SocketTransport_socket_is_not_open;

  int nBytes;
  if ((nBytes = recv(self->socketDescriptor, buffer, bufferLength, 0)) == -1)
    return NDN_ERROR_SocketTransport_error_in_recv;

  *nBytesOut = (size_t)nBytes;

  return NDN_ERROR_success;
}

ndn_Error
ndn_SocketTransport_processEvents
  (struct ndn_SocketTransport *self, uint8_t *buffer, size_t bufferLength)
{
  // Loop until there is no more data in the receive buffer.
  while(1) {
    int receiveIsReady;
    ndn_Error error;
    size_t nBytes;
    if ((error = ndn_SocketTransport_receiveIsReady
         (self, &receiveIsReady)))
      return error;
    if (!receiveIsReady)
      return NDN_ERROR_success;

    if ((error = ndn_SocketTransport_receive
         (self, buffer, bufferLength, &nBytes)))
      return error;
    if (nBytes == 0)
      return NDN_ERROR_success;

    if ((error = ndn_ElementReader_onReceivedData
         (&self->elementReader, buffer, nBytes)))
      return error;
  }
}

ndn_Error ndn_SocketTransport_close(struct ndn_SocketTransport *self)
{
  if (self->socketDescriptor < 0)
    // Already closed.  Do nothing.
    return NDN_ERROR_success;

  if (close(self->socketDescriptor) != 0)
    return NDN_ERROR_SocketTransport_error_in_close;

  self->socketDescriptor = -1;

  return NDN_ERROR_success;
}

#endif // NDN_CPP_HAVE_UNISTD_H
