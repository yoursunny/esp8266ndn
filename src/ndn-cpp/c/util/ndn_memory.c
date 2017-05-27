/**
 * Copyright (C) 2013-2017 Regents of the University of California.
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

#include "ndn_memory.h"

#if !NDN_CPP_HAVE_MEMCMP
int ndn_memcmp(const uint8_t *buf1, const uint8_t *buf2, size_t len)
{
  size_t i;

  for (i = 0; i < len; i++) {
    if (buf1[i] > buf2[i])
      return 1;
    else if (buf1[i] < buf2[i])
      return -1;
  }

  return 0;
}
#else
int ndn_memcmp_stub_to_avoid_empty_file_warning = 0;
#endif

#if !NDN_CPP_HAVE_MEMCPY
void ndn_memcpy(uint8_t *dest, const uint8_t *src, size_t len)
{
  size_t i;

  for (i = 0; i < len; i++)
    dest[i] = src[i];
}
#else
int ndn_memcpy_stub_to_avoid_empty_file_warning = 0;
#endif

#if !NDN_CPP_HAVE_MEMSET
void ndn_memset(uint8_t *dest, int val, size_t len)
{
  size_t i;

  for (i = 0; i < len; i++)
    dest[i] = (uint8_t)val;
}
#else
int ndn_memset_stub_to_avoid_empty_file_warning = 0;
#endif
