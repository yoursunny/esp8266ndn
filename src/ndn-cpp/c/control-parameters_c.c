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

#include "control-parameters.h"

ndn_Error
ndn_ControlParameters_setFromControlParameters
  (struct ndn_ControlParameters *self, const struct ndn_ControlParameters *other)
{
  ndn_Error error;
  if (other == self)
    // Setting to itself. Do nothing.
    return NDN_ERROR_success;

  self->hasName = other->hasName;
  if (other->hasName) {
    if ((error = ndn_Name_setFromName(&self->name, &other->name)))
      return error;
  }
  self->faceId = other->faceId;
  ndn_Blob_setFromBlob(&self->uri, &other->uri);
  self->localControlFeature = other->localControlFeature;
  self->origin = other->origin;
  self->cost = other->cost;
  self->flags = other->flags;
  if ((error = ndn_Name_setFromName(&self->strategy, &other->strategy)))
    return error;
  self->expirationPeriod = other->expirationPeriod;

  return NDN_ERROR_success;
}
