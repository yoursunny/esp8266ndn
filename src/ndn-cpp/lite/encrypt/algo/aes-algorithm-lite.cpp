/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2018 Regents of the University of California.
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

#include "../../../c/encrypt/algo/aes-algorithm.h"
#include "../../../lite/encrypt/algo/aes-algorithm-lite.hpp"

#if NDN_CPP_HAVE_LIBCRYPTO

namespace ndn {

ndn_Error
AesAlgorithmLite::decrypt128Cbc
  (const uint8_t* key, size_t keyLength, const uint8_t* initialVector,
   size_t initialVectorLength, const uint8_t* encryptedData,
   size_t encryptedDataLength, uint8_t* plainData, size_t& plainDataLength)
{
  return ndn_AesAlgorithm_decrypt128Cbc
    (key, keyLength, initialVector, initialVectorLength, encryptedData,
     encryptedDataLength, plainData, &plainDataLength);
}

ndn_Error
AesAlgorithmLite::decrypt128Ecb
  (const uint8_t* key, size_t keyLength, const uint8_t* encryptedData,
   size_t encryptedDataLength, uint8_t* plainData, size_t& plainDataLength)
{
  return ndn_AesAlgorithm_decrypt128Ecb
    (key, keyLength, encryptedData, encryptedDataLength, plainData,
     &plainDataLength);
}

ndn_Error
AesAlgorithmLite::encrypt128Cbc
  (const uint8_t* key, size_t keyLength, const uint8_t* initialVector,
   size_t initialVectorLength, const uint8_t* plainData,
   size_t plainDataLength, uint8_t* encryptedData, size_t& encryptedDataLength)
{
  return ndn_AesAlgorithm_encrypt128Cbc
    (key, keyLength, initialVector, initialVectorLength, plainData,
     plainDataLength, encryptedData, &encryptedDataLength);
}

ndn_Error
AesAlgorithmLite::encrypt128Ecb
  (const uint8_t* key, size_t keyLength, const uint8_t* plainData,
   size_t plainDataLength, uint8_t* encryptedData, size_t& encryptedDataLength)
{
  return ndn_AesAlgorithm_encrypt128Ecb
    (key, keyLength, plainData, plainDataLength, encryptedData,
     &encryptedDataLength);
}

}

#endif // NDN_CPP_HAVE_LIBCRYPTO
