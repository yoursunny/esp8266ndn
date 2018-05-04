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

#ifndef NDN_CONTROL_RESPONSE_LITE_HPP
#define NDN_CONTROL_RESPONSE_LITE_HPP

#include "control-parameters-lite.hpp"
#include "../c/control-response-types.h"

namespace ndn {

/**
 * A ControlResponseLite holds a status code, status text and other fields for a
 * ControlResponse which is used, for example, in the response from sending a
 * register prefix control command to a forwarder.
 */
class ControlResponseLite : private ndn_ControlResponse {
public:
  /**
   * Create a ControlResponseLite to use the pre-allocated nameComponents and
   * strategyNameComponents for the ControlParameters, and with default field
   * values.
   * @param nameComponents The pre-allocated array of ndn_NameComponent for the
   * main name. Instead of an array of NameLite::Component, this is an array of
   * the underlying ndn_NameComponent struct so that it doesn't run the default
   * constructor unnecessarily.
   * @param maxNameComponents The number of elements in the allocated
   * nameComponents array.
   * @param strategyNameComponents The pre-allocated array of ndn_NameComponent
   * for the strategy name. Instead of an array of NameLite::Component, this is
   * an array of the underlying ndn_NameComponent struct so that it doesn't run
   * the default constructor unnecessarily.
   * @param strategyMaxNameComponents The number of elements in the allocated
   * strategyNameComponents array.
   */
  ControlResponseLite
    (struct ndn_NameComponent *nameComponents, size_t maxNameComponents,
     struct ndn_NameComponent *strategyNameComponents,
     size_t strategyMaxNameComponents);

  int
  getStatusCode() const { return statusCode; }

  const BlobLite&
  getStatusText() const { return BlobLite::downCast(statusText); }

  /**
   * Check if the bodyAsControlParameters is specified.
   * @return True if the bodyAsControlParameters is specified, false if not.
   */
  bool
  getHasBodyAsControlParameters() const { return hasBodyAsControlParameters != 0; }

  /**
   * Get the bodyAsControlParameters, if specified.
   * @return The bodyAsControlParameters. This is only meaningful if getHasName() is true.
   */
  ControlParametersLite&
  getBodyAsControlParameters()
  {
    return ControlParametersLite::downCast(bodyAsControlParameters);
  }

  const ControlParametersLite&
  getBodyAsControlParameters() const
  {
    return ControlParametersLite::downCast(bodyAsControlParameters);
  }

  /**
   * Set the flag for whether the bodyAsControlParameters is specified. Note
   * that setBodyAsControlParameters automatically calls setHasName(true).
   * @param hasBodyAsControlParameters True if the bodyAsControlParameters is
   * specified, false if not.
   */
  void
  setHasBodyAsControlParameters(bool hasBodyAsControlParameters)
  {
    this->hasBodyAsControlParameters = hasBodyAsControlParameters ? 1 : 0;
  }

  /**
   * Set the bodyAsControlParameters to have the values from the given control
   * parameters. This also calls setHasBodyAsControlParameters(true).
   * @param bodyAsControlParameters The control parameters to get values from.
   * If the bodyAsControlParameters is not specified, call
   * setBodyAsControlParameters(false).
   * @return 0 for success, or an error code if there is not enough room in this
   * object's name components array.
   */
  ndn_Error
  setBodyAsControlParameters(const ControlParametersLite& bodyAsControlParameters)
  {
    hasBodyAsControlParameters = 1;
    return ControlParametersLite::downCast(this->bodyAsControlParameters)
      .set(bodyAsControlParameters);
  }

  /**
   * Set the status code.
   * @param statusCode The new status code, or -1 for not specified.
   * @return This ControlResponseLite so that you can chain calls to update
   * values.
   */
  ControlResponseLite&
  setStatusCode(int statusCode)
  {
    this->statusCode = statusCode;
    return *this;
  }

  /**
   * Set status text.
   * @param statusText The new status text, or an empty string for not specified.
   * @return This ControlResponseLite so that you can chain calls to update
   * values.
   */
  ControlResponseLite&
  setStatusText(const BlobLite& statusText)
  {
    BlobLite::downCast(this->statusText) = statusText;
    return *this;
  }

  /**
   * Downcast the reference to the ndn_ControlResponse struct to a
   * ControlResponseLite.
   * @param controlResponse A reference to the ndn_ControlResponse struct.
   * @return The same reference as ControlResponseLite.
   */
  static ControlResponseLite&
  downCast(ndn_ControlResponse& controlResponse)
  {
    return *(ControlResponseLite*)&controlResponse;
  }

  static const ControlResponseLite&
  downCast(const ndn_ControlResponse& controlResponse)
  {
    return *(ControlResponseLite*)&controlResponse;
  }

private:
  // Declare friends who can downcast to the private base.
  friend class Tlv0_2WireFormatLite;
};

}

#endif
