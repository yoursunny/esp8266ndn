/**
 * Copyright (C) 2016 Regents of the University of California.
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

#ifndef NDN_CONTROL_RESPONSE_H
#define NDN_CONTROL_RESPONSE_H

#include <ndn-cpp/c/control-response-types.h>
#include "control-parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize an ndn_ControlResponse struct with the pre-allocated
 * nameComponents used for the ControlParameters, and defaults for all the values.
 * @param self A pointer to the ndn_ControlResponse struct.
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
ndn_ControlResponse_initialize
  (struct ndn_ControlResponse *self, struct ndn_NameComponent *nameComponents,
   size_t maxNameComponents, struct ndn_NameComponent *strategyNameComponents,
   size_t strategyMaxNameComponents)
{
  self->statusCode = -1;
  ndn_Blob_initialize(&self->statusText, 0, 0);
  self->hasBodyAsControlParameters = 0;
  ndn_ControlParameters_initialize
    (&self->bodyAsControlParameters, nameComponents, maxNameComponents,
     strategyNameComponents, strategyMaxNameComponents);
}

#ifdef __cplusplus
}
#endif

#endif
