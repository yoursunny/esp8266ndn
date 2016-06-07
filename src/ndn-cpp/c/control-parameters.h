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

#ifndef NDN_CONTROL_PARAMETERS_OPTIONS_H
#define NDN_CONTROL_PARAMETERS_OPTIONS_H

#include <ndn-cpp/c/control-parameters-types.h>
#include "forwarding-flags-impl.h"
#include "name.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_ControlParameters struct with the pre-allocated
 * nameComponents, and defaults for all the values.
 * @param self A pointer to the ndn_ControlParameters struct.
 * @param nameComponents the pre-allocated array of ndn_NameComponent for the
 * main name.
 * @param maxNameComponents the number of elements in the allocated
 * nameComponents array.
 * @param strategyNameComponents the pre-allocated array of ndn_NameComponent
 * for the strategy name.
 * @param strategyMaxNameComponents the number of elements in the allocated
 * strategyNameComponents array.
 */
static __inline void
ndn_ControlParameters_initialize
  (struct ndn_ControlParameters *self, struct ndn_NameComponent *nameComponents,
   size_t maxNameComponents, struct ndn_NameComponent *strategyNameComponents,
   size_t strategyMaxNameComponents)
{
  self->hasName = 0;
  ndn_Name_initialize(&self->name, nameComponents, maxNameComponents);
  self->faceId = -1;
  ndn_Blob_initialize(&self->uri, 0, 0);
  self->localControlFeature = -1;
  self->origin = -1;
  self->cost = -1;
  ndn_ForwardingFlags_initialize(&self->flags);
  ndn_Name_initialize
    (&self->strategy, strategyNameComponents, strategyMaxNameComponents);
  self->expirationPeriod = -1.0;
}

/**
 * Set this control parameters to have the values from the other control
 * parameters.
 * @param self A pointer to the ndn_ControlParameters struct.
 * @param other A pointer to the other ndn_ControlParameters struct to get
 * values from.
 * @return 0 for success, or an error code if there is not enough room in this
 * object's components array.
 */
ndn_Error
ndn_ControlParameters_setFromControlParameters
  (struct ndn_ControlParameters *self, const struct ndn_ControlParameters *other);

#ifdef __cplusplus
}
#endif

#endif
