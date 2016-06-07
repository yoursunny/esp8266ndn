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

#include "../../c/util/crypto.h"

#ifdef ARDUINO
// Put the ARDUINO implementations in this C++ file, not in crypto.c.

#include "../../../contrib/cryptosuite/sha256.h"

void
ndn_digestSha256(const uint8_t *data, size_t dataLength, uint8_t *digest)
{
  // The Arduino is single-threaded, so use the global Sha256 object.
  Sha256.init();
  for (size_t i = 0; i < dataLength; ++i)
    Sha256.write(data[i]);
  memcpy(digest, Sha256.result(), ndn_SHA256_DIGEST_SIZE);
}

void
ndn_computeHmacWithSha256
  (const uint8_t *key, size_t keyLength, const uint8_t *data, size_t dataLength,
   uint8_t *digest)
{
  // The Arduino is single-threaded, so use the global Sha256 object.
  Sha256.initHmac(key, keyLength);
  for (size_t i = 0; i < dataLength; ++i)
    Sha256.write(data[i]);
  memcpy(digest, Sha256.resultHmac(), ndn_SHA256_DIGEST_SIZE);
}

#endif
