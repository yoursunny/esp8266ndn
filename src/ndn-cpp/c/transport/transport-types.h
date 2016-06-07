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

#ifndef NDN_TRANSPORT_TYPES_H
#define NDN_TRANSPORT_TYPES_H

#include "../encoding/element-reader-types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ndn_SocketTransport {
  int socketDescriptor; /**< -1 if not connected */
  struct ndn_ElementReader elementReader;
};

struct ndn_TcpTransport {
  struct ndn_SocketTransport base;
};

struct ndn_UdpTransport {
  struct ndn_SocketTransport base;
};

struct ndn_UnixTransport {
  struct ndn_SocketTransport base;
};

#ifdef __cplusplus
}
#endif

#endif
