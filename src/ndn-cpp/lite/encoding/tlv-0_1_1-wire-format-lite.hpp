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

#ifndef NDN_TLV_0_1_1_WIRE_FORMAT_LITE_HPP
#define NDN_TLV_0_1_1_WIRE_FORMAT_LITE_HPP

#include "tlv-0_2-wire-format-lite.hpp"

namespace ndn {

/**
 * A Tlv0_1_1WireFormatLite extends Tlv0_2WireFormatLite so that it is an alias
 * in case any applications use Tlv0_1_1WireFormatLite directly.  These two wire
 * formats are the same except that Tlv0_2WireFormatLite adds support for the
 * name component type ImplicitSha256Digest.
 */
class Tlv0_1_1WireFormatLite : public Tlv0_2WireFormatLite {
};

}

#endif
