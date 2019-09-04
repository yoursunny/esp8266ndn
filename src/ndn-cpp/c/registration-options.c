/**
 * Copyright (C) 2019 Regents of the University of California.
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
#include "registration-options.h"

void ndn_RegistrationOptions_initialize(struct ndn_RegistrationOptions *self)
{
  self->childInherit = 1;
  self->capture = 0;
  self->origin = -1;
}

int
ndn_RegistrationOptions_getNfdForwardingFlags(const struct ndn_RegistrationOptions *self)
{
  int result = 0;

  if (self->childInherit)
    result |= ndn_NfdForwardingFlags_CHILD_INHERIT;
  if (self->capture)
    result |= ndn_NfdForwardingFlags_CAPTURE;

  return result;
}

void
ndn_RegistrationOptions_setNfdForwardingFlags
  (struct ndn_RegistrationOptions *self, int nfdForwardingFlags)
{
  self->childInherit = (nfdForwardingFlags & ndn_NfdForwardingFlags_CHILD_INHERIT) ? 1 : 0;
  self->capture = (nfdForwardingFlags & ndn_NfdForwardingFlags_CAPTURE) ? 1 : 0;
}
