/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2018 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-group-encrypt src/encrypt-params https://github.com/named-data/ndn-group-encrypt
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

#ifndef NDN_ENCRYPT_PARAMS_LITE_HPP
#define NDN_ENCRYPT_PARAMS_LITE_HPP

#include "../../util/blob-lite.hpp"
#include "../../../c/encrypt/algo/encrypt-params-types.h"

namespace ndn {

/**
 * An EncryptParamsLite holds an algorithm type and other parameters used to
 * encrypt and decrypt.
 * @note This class is an experimental feature. The API may change.
 */
class EncryptParamsLite : private ndn_EncryptParams {
public:
  /**
   * Create an EncryptParamsLite with defaults for all the values.
   */
  EncryptParamsLite();

  /**
   * Get the algorithm type.
   * @return The algorithm type, or -1 if not specified.
   */
  ndn_EncryptAlgorithmType
  getAlgorithmType() const { return algorithmType; }

  /**
   * Get the initial vector.
   * @return The initial vector. If not specified, isNull() is true.
   */
  const BlobLite&
  getInitialVector() const { return BlobLite::downCast(initialVector); }

  /**
   * Set the algorithm type.
   * @param algorithmType The algorithm type. If not specified, set to -1.
   * @return This EncryptParamsLite so that you can chain calls to update values.
   */
  EncryptParamsLite&
  setAlgorithmType(ndn_EncryptAlgorithmType algorithmType)
  {
    this->algorithmType = algorithmType;
    return *this;
  }

  /**
   * Set the initial vector.
   * @param initialVector The initial vector. If not specified, set to the
   * default BlobLite() where isNull() is true.
   * @return This EncryptParamsLite so that you can chain calls to update values.
   */
  EncryptParamsLite&
  setInitialVector(const BlobLite& initialVector)
  {
    BlobLite::downCast(this->initialVector) = initialVector;
    return *this;
  }

  /**
   * Downcast the reference to the ndn_EncryptParams struct to an EncryptParamsLite.
   * @param keyLocator A reference to the ndn_EncryptParams struct.
   * @return The same reference as EncryptParamsLite.
   */
  static EncryptParamsLite&
  downCast(ndn_EncryptParams& encryptParams) { return *(EncryptParamsLite*)&encryptParams; }

  static const EncryptParamsLite&
  downCast(const ndn_EncryptParams& encryptParams) { return *(EncryptParamsLite*)&encryptParams; }
};

}

#endif
