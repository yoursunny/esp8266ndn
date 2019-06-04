/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2019 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * @author: From ndn-cxx packet.hpp https://github.com/named-data/ndn-cxx/blob/master/ndn-cxx/lp/packet.hpp
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

#ifndef NDN_LPPACKETLITE_HPP
#define NDN_LPPACKETLITE_HPP

#include "../util/blob-lite.hpp"
#include "incoming-face-id-lite.hpp"
#include "../network-nack-lite.hpp"
#include "congestion-mark-lite.hpp"
#include "../../c/lp/lp-packet-types.h"

namespace ndn {

class LpPacketHeaderFieldLite : private ndn_LpPacketHeaderField {
public:
  /**
   * Get the type of this header field.
   * @return The header field type.
   */
  ndn_LpPacketHeaderFieldType
  getType() const { return type; }

  /**
   * Get this field as an IncomingFaceId. This assumes you checked that
   * getType() is ndn_LpPacketHeaderFieldType_INCOMING_FACE_ID.
   * @return This field as an IncomingFaceId.
   */
  const IncomingFaceIdLite&
  getIncomingFaceId() const { return IncomingFaceIdLite::downCast(incomingFaceId); }

  IncomingFaceIdLite&
  getIncomingFaceId() { return IncomingFaceIdLite::downCast(incomingFaceId); }

  /**
   * Get this field as an NetworkNack. This assumes you checked that
   * getType() is ndn_LpPacketHeaderFieldType_NETWORK_NACK.
   * @return This field as an NetworkNack.
   */
  const NetworkNackLite&
  getNetworkNack() const { return NetworkNackLite::downCast(networkNack); }

  NetworkNackLite&
  getNetworkNack() { return NetworkNackLite::downCast(networkNack); }

  /**
   * Get this field as a CongestionMark. This assumes you checked that
   * getType() is ndn_LpPacketHeaderFieldType_CONGESTION_MARK.
   * @return This field as a CongestionMark.
   */
  const CongestionMarkLite&
  getCongestionMark() const { return CongestionMarkLite::downCast(congestionMark); }

  CongestionMarkLite&
  getCongestionMark() { return CongestionMarkLite::downCast(congestionMark); }

  /**
   * Downcast the reference to the ndn_LpPacketHeaderField struct to an
   * LpPacketHeaderFieldLite.
   * @param lpPacketHeaderField A reference to the ndn_LpPacketHeaderField struct.
   * @return The same reference as LpPacketHeaderFieldLite.
   */
  static LpPacketHeaderFieldLite&
  downCast(ndn_LpPacketHeaderField& lpPacketHeaderField)
  {
    return *(LpPacketHeaderFieldLite*)&lpPacketHeaderField;
  }

  static const LpPacketHeaderFieldLite&
  downCast(const ndn_LpPacketHeaderField& lpPacketHeaderField)
  {
    return *(LpPacketHeaderFieldLite*)&lpPacketHeaderField;
  }
};

class LpPacketLite : private ndn_LpPacket {
public:
  /**
   * Create an LpPacket to use the headerFieldsArray.
   * @param headerFields The pre-allocated array of ndn_LpPacketHeaderField.
   * This is an array of the underlying ndn_LpPacketHeaderField struct so that
   * it doesn't run a default constructor unnecessarily.
   * @param maxHeaderFields The number of elements in the allocated headerFields
   * array.
   */
  LpPacketLite
    (struct ndn_LpPacketHeaderField* headerFields, size_t maxHeaderFields);

  /**
   * Get the fragment wire encoding.
   * @return The wire encoding, or an isNull Blob if not specified.
   */
  const BlobLite&
  getFragmentWireEncoding() const { return BlobLite::downCast(fragmentWireEncoding); }

  /**
   * Get the number of header fields. This does not include the fragment.
   * @return The number of header fields.
   */
  size_t
  countHeaderFields() const { return nHeaderFields; }

  /**
   * Get the header field at the given index.
   * @param index The index, starting from 0. It is an error if index is greater
   * to or equal to countHeaderFields().
   * @return The header field at the index.
   */
  const LpPacketHeaderFieldLite&
  getHeaderField(int index) const
  {
    return LpPacketHeaderFieldLite::downCast(headerFields[index]);
  }

  LpPacketHeaderFieldLite&
  getHeaderField(int index)
  {
    return LpPacketHeaderFieldLite::downCast(headerFields[index]);
  }

  /**
   * Set the fragment wire encoding.
   * @param fragmentWireEncoding The fragment wire encoding or an isNull
   * BlobLite if not specified.
   */
  void
  setFragmentWireEncoding(const BlobLite& fragmentWireEncoding)
  {
    BlobLite::downCast(this->fragmentWireEncoding) = fragmentWireEncoding;
  }

  /**
   * Downcast the reference to the ndn_LpPacket struct to an LpPacketLite.
   * @param lpPacket A reference to the ndn_LpPacket struct.
   * @return The same reference as LpPacketLite.
   */
  static LpPacketLite&
  downCast(ndn_LpPacket& lpPacket) { return *(LpPacketLite*)&lpPacket; }

  static const LpPacketLite&
  downCast(const ndn_LpPacket& lpPacket) { return *(LpPacketLite*)&lpPacket; }

private:
  // Declare friends who can downcast to the private base.
  friend class Tlv0_2WireFormatLite;
  friend class IncomingFaceIdLite;
  friend class NetworkNackLite;
  friend class CongestionMarkLite;

  /**
   * Don't allow the copy constructor. Instead use set(const LpPacketLite&) which
   * can return an error if there is no more room in the headerFields array.
   */
  LpPacketLite(const LpPacketLite& other);

  /**
   * Don't allow the assignment operator. Instead use set(const LpPacketLite&) which
   * can return an error if there is no more room in the headerFields array.
   */
  LpPacketLite& operator=(const LpPacketLite& other);
};

}

#endif
