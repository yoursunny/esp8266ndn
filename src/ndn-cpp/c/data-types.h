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

#ifndef NDN_DATA_TYPES_H
#define NDN_DATA_TYPES_H

#include "key-types.h"
#include "security/validity-period-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ndn_ContentType specifies the content type in a MetaInfo object. If the
 * content type in the packet is not a recognized enum value, then we use
 * ndn_ContentType_OTHER_CODE and you can call MetaInfo::getOtherTypeCode(). We
 * do this to keep the recognized content type values independent of packet
 * encoding formats. Note that the constants for BLOB, LINK, KEY and NACK are
 * the same as defined in the NDN-TLV spec.
 */
typedef enum {
  ndn_ContentType_BLOB = 0,
  ndn_ContentType_LINK = 1,
  ndn_ContentType_KEY =  2,
  ndn_ContentType_NACK = 3,
  ndn_ContentType_OTHER_CODE = 0x7fff
} ndn_ContentType;

/** ndn_SignatureType defines constants for the Signature "type" field.
 * Note that the constants are the same as defined in the NDN-TLV spec, but we
 * keep a separate enum so that we aren't directly tied to the TLV code.
 */
typedef enum {
  ndn_SignatureType_DigestSha256Signature = 0,
  ndn_SignatureType_Sha256WithRsaSignature = 1,
  ndn_SignatureType_Sha256WithEcdsaSignature = 3,
  ndn_SignatureType_HmacWithSha256Signature = 4,
  ndn_SignatureType_Generic = 0x7fff
} ndn_SignatureType;

/**
 * An ndn_Signature struct holds the signature bits and other info representing
 * the signature in a data packet or signed interest. We use one structure which
 * is a union of all the fields from the different known signature types. This
 * lets us avoid the infrastructure to simulate an abstract base class with
 * subclasses and virtual methods.
 */
struct ndn_Signature {
  ndn_SignatureType type;                /**< -1 for unspecified */
  struct ndn_Blob signature;
  struct ndn_Blob signatureInfoEncoding; /**< used with Generic */
  int genericTypeCode;                   /**< used with Generic. -1 for not known. */
  struct ndn_KeyLocator keyLocator; /**< used with Sha256WithRsaSignature,
                                     * Sha256WithEcdsaSignature, HmacWithSha256Signature */
  struct ndn_ValidityPeriod validityPeriod; /**< used with Sha256WithRsaSignature,
                                     * Sha256WithEcdsaSignature */
};

/**
 * An ndn_MetaInfo struct holds the meta info which is signed inside the data packet.
 */
struct ndn_MetaInfo {
  ndn_MillisecondsSince1970 timestampMilliseconds; /**< milliseconds since 1/1/1970. -1 for none */
  ndn_ContentType type;                  /**< default is ndn_ContentType_BLOB. -1 for none */
  int otherTypeCode;
  ndn_Milliseconds freshnessPeriod;      /**< -1 for none */
  struct ndn_NameComponent finalBlockId; /**< has a pointer to a pre-allocated buffer.  0 for none */
};

struct ndn_Data {
  struct ndn_Signature signature;
  struct ndn_Name name;
  struct ndn_MetaInfo metaInfo;
  struct ndn_Blob content;     /**< A Blob with a pointer to the content. */
};

#ifdef __cplusplus
}
#endif

#endif
