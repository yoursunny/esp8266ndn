/**
 * Copyright (C) 2013-2018 Regents of the University of California.
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
#include "forwarding-flags-impl.h"

void ndn_ForwardingFlags_initialize(struct ndn_ForwardingFlags *self)
{
  self->childInherit = 1;
  self->capture = 0;
}

int
ndn_ForwardingFlags_getNfdForwardingFlags(const struct ndn_ForwardingFlags *self)
{
  int result = 0;

  if (self->childInherit)
    result |= ndn_NfdForwardingFlags_CHILD_INHERIT;
  if (self->capture)
    result |= ndn_NfdForwardingFlags_CAPTURE;

  return result;
}

void
ndn_ForwardingFlags_setNfdForwardingFlags
  (struct ndn_ForwardingFlags *self, int nfdForwardingFlags)
{
  self->childInherit = (nfdForwardingFlags & ndn_NfdForwardingFlags_CHILD_INHERIT) ? 1 : 0;
  self->capture = (nfdForwardingFlags & ndn_NfdForwardingFlags_CAPTURE) ? 1 : 0;
}
