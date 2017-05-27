/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2017 Regents of the University of California.
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

#include "../../c/security/ec-private-key.h"
#include "../../lite/security/ec-private-key-lite.hpp"

namespace ndn {

EcPrivateKeyLite::EcPrivateKeyLite()
{
  ndn_EcPrivateKey_initialize(this);
}

EcPrivateKeyLite::~EcPrivateKeyLite()
{
  ndn_EcPrivateKey_finalize(this);
}

ndn_Error
EcPrivateKeyLite::decode
  (const uint8_t* privateKeyDer, size_t privateKeyDerLength)
{
  return ndn_EcPrivateKey_decode(this, privateKeyDer, privateKeyDerLength);
}

ndn_Error
EcPrivateKeyLite::setByCurve
  (int curveId, const uint8_t* value, size_t valueLength)
{
  return ndn_EcPrivateKey_setByCurve(this, curveId, value, valueLength);
}

ndn_Error
EcPrivateKeyLite::generate(uint32_t keySize)
{
  return ndn_EcPrivateKey_generate(this, keySize);
}

ndn_Error
EcPrivateKeyLite::encodePrivateKey
  (bool includeParameters, uint8_t* encoding, size_t& encodingLength) const
{
  return ndn_EcPrivateKey_encodePrivateKey
    (this, includeParameters ? 1 : 0, encoding, &encodingLength);
}

ndn_Error
EcPrivateKeyLite::encodePublicKey
  (bool includeParameters, uint8_t* encoding, size_t& encodingLength) const
{
  return ndn_EcPrivateKey_encodePublicKey
    (this, includeParameters ? 1 : 0. , encoding, &encodingLength);
}

ndn_Error
EcPrivateKeyLite::signWithSha256
  (const uint8_t* data, size_t dataLength, uint8_t* signature,
   size_t& signatureLength) const
{
  return ndn_EcPrivateKey_signWithSha256
    (this, data, dataLength, signature, &signatureLength);
}

}
