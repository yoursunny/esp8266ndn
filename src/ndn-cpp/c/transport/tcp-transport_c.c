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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <string.h>
#include "../util/ndn_memory.h"
#include <errno.h>
#include "tcp-transport.h"

ndn_Error
ndn_TcpTransport_isLocal(const char *host, int *result)
{
  // Imitate ndn_SocketTransport_connect to use getaddrinfo.
  struct addrinfo hints;
  struct addrinfo *serverInfo;
  char ipString[INET6_ADDRSTRLEN];

  ndn_memset((uint8_t *)&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(host, NULL, &hints, &serverInfo) != 0)
    return NDN_ERROR_SocketTransport_error_in_getaddrinfo;

  // Only look at the first serverInfo.
  if (serverInfo->ai_family == AF_INET) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)serverInfo->ai_addr;
    inet_ntop(serverInfo->ai_family, &ipv4->sin_addr, ipString, sizeof(ipString));

    *result = (ndn_memcmp((const uint8_t *)ipString,
                          (const uint8_t *)"127.", 4) == 0 ? 1 : 0);
  }
  else {
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)serverInfo->ai_addr;
    inet_ntop(serverInfo->ai_family,  &ipv6->sin6_addr, ipString, sizeof(ipString));

    *result = (strcmp(ipString, "::1") == 0 ? 1 : 0);
  }

  freeaddrinfo(serverInfo);
  return NDN_ERROR_success;
}

#endif // NDN_CPP_HAVE_UNISTD_H
