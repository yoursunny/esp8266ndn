/**
 * Copyright (C) 2014-2017 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 * Derived from tlv.hpp by Alexander Afanasyev <alexander.afanasyev@ucla.edu>
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

#ifndef NDN_TLV_H
#define NDN_TLV_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
  ndn_Tlv_Interest =         5,
  ndn_Tlv_Data =             6,
  ndn_Tlv_Name =             7,
  ndn_Tlv_ImplicitSha256DigestComponent = 1,
  ndn_Tlv_NameComponent =    8,
  ndn_Tlv_Selectors =        9,
  ndn_Tlv_Nonce =            10,
  // ndn_Tlv_<Unassigned> =  11,
  ndn_Tlv_InterestLifetime = 12,
  ndn_Tlv_MinSuffixComponents = 13,
  ndn_Tlv_MaxSuffixComponents = 14,
  ndn_Tlv_PublisherPublicKeyLocator = 15,
  ndn_Tlv_Exclude =          16,
  ndn_Tlv_ChildSelector =    17,
  ndn_Tlv_MustBeFresh =      18,
  ndn_Tlv_Any =              19,
  ndn_Tlv_MetaInfo =         20,
  ndn_Tlv_Content =          21,
  ndn_Tlv_SignatureInfo =    22,
  ndn_Tlv_SignatureValue =   23,
  ndn_Tlv_ContentType =      24,
  ndn_Tlv_FreshnessPeriod =  25,
  ndn_Tlv_FinalBlockId =     26,
  ndn_Tlv_SignatureType =    27,
  ndn_Tlv_KeyLocator =       28,
  ndn_Tlv_KeyLocatorDigest = 29,
  ndn_Tlv_SelectedDelegation = 32
};

enum {
  ndn_Tlv_SignatureType_DigestSha256 = 0,
  ndn_Tlv_SignatureType_SignatureSha256WithRsa = 1,
  ndn_Tlv_SignatureType_SignatureSha256WithEcdsa = 3,
  ndn_Tlv_SignatureType_SignatureHmacWithSha256 = 4
};

enum {
  ndn_Tlv_ContentType_Default = 0,
  ndn_Tlv_ContentType_Link = 1,
  ndn_Tlv_ContentType_Key = 2
};

enum {
  ndn_Tlv_NfdCommand_ControlResponse = 101,
  ndn_Tlv_NfdCommand_StatusCode =      102,
  ndn_Tlv_NfdCommand_StatusText =      103
};

enum {
  ndn_Tlv_ControlParameters_ControlParameters =   104,
  ndn_Tlv_ControlParameters_FaceId =              105,
  ndn_Tlv_ControlParameters_Uri =                 114,
  ndn_Tlv_ControlParameters_LocalControlFeature = 110,
  ndn_Tlv_ControlParameters_Origin =              111,
  ndn_Tlv_ControlParameters_Cost =                106,
  ndn_Tlv_ControlParameters_Flags =               108,
  ndn_Tlv_ControlParameters_Strategy =            107,
  ndn_Tlv_ControlParameters_ExpirationPeriod =    109
};

enum {
  ndn_Tlv_LpPacket_LpPacket =        100,
  ndn_Tlv_LpPacket_Fragment =         80,
  ndn_Tlv_LpPacket_Sequence =         81,
  ndn_Tlv_LpPacket_FragIndex =        82,
  ndn_Tlv_LpPacket_FragCount =        83,
  ndn_Tlv_LpPacket_Nack =            800,
  ndn_Tlv_LpPacket_NackReason =      801,
  ndn_Tlv_LpPacket_NextHopFaceId =   816,
  ndn_Tlv_LpPacket_IncomingFaceId =  817,
  ndn_Tlv_LpPacket_CachePolicy =     820,
  ndn_Tlv_LpPacket_CachePolicyType = 821,
  ndn_Tlv_LpPacket_IGNORE_MIN =      800,
  ndn_Tlv_LpPacket_IGNORE_MAX =      959
};

enum {
  ndn_Tlv_Link_Preference = 30,
  ndn_Tlv_Link_Delegation = 31
};

enum {
  ndn_Tlv_Encrypt_EncryptedContent =    130,
  ndn_Tlv_Encrypt_EncryptionAlgorithm = 131,
  ndn_Tlv_Encrypt_EncryptedPayload =    132,
  ndn_Tlv_Encrypt_InitialVector =       133
};

// For RepetitiveInterval.
enum {
  ndn_Tlv_Encrypt_StartDate =          134,
  ndn_Tlv_Encrypt_EndDate =            135,
  ndn_Tlv_Encrypt_IntervalStartHour =  136,
  ndn_Tlv_Encrypt_IntervalEndHour =    137,
  ndn_Tlv_Encrypt_NRepeats =           138,
  ndn_Tlv_Encrypt_RepeatUnit =         139,
  ndn_Tlv_Encrypt_RepetitiveInterval = 140
};

// For Schedule.
enum {
  ndn_Tlv_Encrypt_WhiteIntervalList = 141,
  ndn_Tlv_Encrypt_BlackIntervalList = 142,
  ndn_Tlv_Encrypt_Schedule =          143
};

enum {
  ndn_Tlv_ValidityPeriod_ValidityPeriod = 253,
  ndn_Tlv_ValidityPeriod_NotBefore =      254,
  ndn_Tlv_ValidityPeriod_NotAfter =       255
};

#ifdef __cplusplus
}
#endif

#endif
