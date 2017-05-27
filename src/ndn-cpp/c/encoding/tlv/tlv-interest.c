/**
 * Copyright (C) 2014-2017 Regents of the University of California.
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

#include <math.h>
#include "tlv-name.h"
#include "tlv-key-locator.h"
#include "../../util/crypto.h"
#include "tlv-interest.h"

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs in the body of the Exclude value.
 * @param context This is the ndn_Exclude struct pointer which was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeExcludeValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_Exclude *exclude = (struct ndn_Exclude *)context;

  // TODO: Do we want to order the components (except for ANY)?
  ndn_Error error;
  size_t i;
  for (i = 0; i < exclude->nEntries; ++i) {
    struct ndn_ExcludeEntry *entry = &exclude->entries[i];

    if (entry->type == ndn_Exclude_COMPONENT) {
      if ((error = ndn_encodeTlvNameComponent(&entry->component, encoder)))
        return error;
    }
    else if (entry->type == ndn_Exclude_ANY) {
      if ((error = ndn_TlvEncoder_writeTypeAndLength(encoder, ndn_Tlv_Any, 0)))
        return error;
    }
    else
      return NDN_ERROR_unrecognized_ndn_ExcludeType;
  }

  return NDN_ERROR_success;
}

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs in the body of the Selectors value.
 * @param context This is the ndn_Interest struct pointer which was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeSelectorsValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  struct ndn_Interest *interest = (struct ndn_Interest *)context;
  ndn_Error error;

  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv
      (encoder, ndn_Tlv_MinSuffixComponents, interest->minSuffixComponents)))
    return error;
  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv
      (encoder, ndn_Tlv_MaxSuffixComponents, interest->maxSuffixComponents)))
    return error;

  if ((error = ndn_TlvEncoder_writeNestedTlv
       (encoder, ndn_Tlv_PublisherPublicKeyLocator, ndn_encodeTlvKeyLocatorValue,
        &interest->keyLocator, 1)))
    return error;

  if (interest->exclude.nEntries > 0) {
    if ((error = ndn_TlvEncoder_writeNestedTlv(encoder, ndn_Tlv_Exclude, encodeExcludeValue, &interest->exclude, 0)))
      return error;
  }

  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv
      (encoder, ndn_Tlv_ChildSelector, interest->childSelector)))
    return error;

  if (interest->mustBeFresh) {
    if ((error = ndn_TlvEncoder_writeTypeAndLength(encoder, ndn_Tlv_MustBeFresh, 0)))
      return error;
  }
  // else MustBeFresh == false, so nothing to encode.

  return NDN_ERROR_success;
}

/* An InterestValueContext is for passing the context to encodeInterestValue so
 *   that we can include signedPortionBeginOffset and signedPortionEndOffset.
 */
struct InterestValueContext {
  const struct ndn_Interest *interest;
  size_t *signedPortionBeginOffset;
  size_t *signedPortionEndOffset;
};

/**
 * This private function is called by ndn_TlvEncoder_writeNestedTlv to write the TLVs in the body of the Interest value.
 * @param context This is the InterestValueContext struct pointer which was passed to writeTlv.
 * @param encoder the ndn_TlvEncoder which is calling this.
 * @return 0 for success, else an error code.
 */
static ndn_Error
encodeInterestValue(const void *context, struct ndn_TlvEncoder *encoder)
{
  const struct InterestValueContext *interestValueContext =
    (const struct InterestValueContext *)context;
  const struct ndn_Interest *interest = interestValueContext->interest;
  ndn_Error error;
  uint8_t nonceBuffer[4];
  struct ndn_Blob nonceBlob;

  if ((error = ndn_encodeTlvName
       (&interest->name, interestValueContext->signedPortionBeginOffset,
        interestValueContext->signedPortionEndOffset, encoder)))
    return error;
  // For Selectors, set omitZeroLength true.
  if ((error = ndn_TlvEncoder_writeNestedTlv(encoder, ndn_Tlv_Selectors, encodeSelectorsValue, interest, 1)))
    return error;

  // Encode the Nonce as 4 bytes.
  nonceBlob.length = sizeof(nonceBuffer);
  if (interest->nonce.length == 0) {
    // Generate a random nonce.
    if ((error = ndn_generateRandomBytes(nonceBuffer, sizeof(nonceBuffer))))
      return error;
    nonceBlob.value = nonceBuffer;
  }
  else if (interest->nonce.length < 4) {
    // TLV encoding requires 4 bytes, so pad out to 4 using random bytes.
    ndn_memcpy(nonceBuffer, interest->nonce.value, interest->nonce.length);
    if ((error = ndn_generateRandomBytes
         (nonceBuffer + interest->nonce.length,
          sizeof(nonceBuffer) - interest->nonce.length)))
      return error;
    nonceBlob.value = nonceBuffer;
  }
  else
    // TLV encoding requires 4 bytes, so truncate to 4.
    nonceBlob.value = interest->nonce.value;
  if ((error = ndn_TlvEncoder_writeBlobTlv(encoder, ndn_Tlv_Nonce, &nonceBlob)))
    return error;

  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlvFromDouble
      (encoder, ndn_Tlv_InterestLifetime, interest->interestLifetimeMilliseconds)))
    return error;

  if (interest->linkWireEncoding.value) {
    // Encode the entire link as is.
    if ((error = ndn_TlvEncoder_writeArray
        (encoder, interest->linkWireEncoding.value, interest->linkWireEncoding.length)))
      return error;
  }
  if ((error = ndn_TlvEncoder_writeOptionalNonNegativeIntegerTlv
      (encoder, ndn_Tlv_SelectedDelegation, interest->selectedDelegationIndex)))
    return error;

  return NDN_ERROR_success;
}

ndn_Error
ndn_encodeTlvInterest
  (const struct ndn_Interest *interest, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_TlvEncoder *encoder)
{
  // Create the context to pass to encodeInterestValue.
  struct InterestValueContext interestValueContext;
  interestValueContext.interest = interest;
  interestValueContext.signedPortionBeginOffset = signedPortionBeginOffset;
  interestValueContext.signedPortionEndOffset = signedPortionEndOffset;

  return ndn_TlvEncoder_writeNestedTlv
    (encoder, ndn_Tlv_Interest, encodeInterestValue, &interestValueContext, 0);
}

static ndn_Error
decodeExclude(struct ndn_Exclude *exclude, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, ndn_Tlv_Exclude, &endOffset)))
    return error;

  exclude->nEntries = 0;
  while (decoder->offset < endOffset) {
    int gotExpectedTag;

    if ((error = ndn_TlvDecoder_peekType
         (decoder, ndn_Tlv_Any, endOffset, &gotExpectedTag)))
      return error;
    if (gotExpectedTag) {
      // Read past the Any TLV.
      int dummyValue;
      if ((error = ndn_TlvDecoder_readBooleanTlv
           (decoder, ndn_Tlv_Any, endOffset, &dummyValue)))
        return error;
      if ((error = ndn_Exclude_appendAny(exclude)))
        return error;
    }
    else {
      // First append an empty component, then decode into it.
      if ((error = ndn_Exclude_appendComponent(exclude, 0, 0)))
        return error;
      if ((error = ndn_decodeTlvNameComponent
           (&exclude->entries[exclude->nEntries - 1].component, decoder)))
        return error;
      exclude->entries[exclude->nEntries - 1].type = ndn_Exclude_COMPONENT;
    }
  }

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}

static ndn_Error
decodeSelectors(struct ndn_Interest *interest, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  int gotExpectedType;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, ndn_Tlv_Selectors, &endOffset)))
    return error;

  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_MinSuffixComponents, endOffset, &interest->minSuffixComponents)))
    return error;
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_MaxSuffixComponents, endOffset, &interest->maxSuffixComponents)))
    return error;

  if ((error = ndn_TlvDecoder_peekType
       (decoder, ndn_Tlv_PublisherPublicKeyLocator, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    if ((error = ndn_decodeTlvKeyLocator
         (ndn_Tlv_PublisherPublicKeyLocator, &interest->keyLocator, decoder)))
      return error;
  }
  else
    // Clear the key locator.
    ndn_KeyLocator_initialize(&interest->keyLocator, interest->keyLocator.keyName.components, interest->keyLocator.keyName.maxComponents);


  if ((error = ndn_TlvDecoder_peekType(decoder, ndn_Tlv_Exclude, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    if ((error = decodeExclude(&interest->exclude, decoder)))
      return error;
  }
  else
    interest->exclude.nEntries = 0;

  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_ChildSelector, endOffset, &interest->childSelector)))
    return error;

  if ((error = ndn_TlvDecoder_readBooleanTlv
       (decoder, ndn_Tlv_MustBeFresh, endOffset, &interest->mustBeFresh)))
    return error;

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}

ndn_Error
ndn_decodeTlvInterest
  (struct ndn_Interest *interest, size_t *signedPortionBeginOffset,
   size_t *signedPortionEndOffset, struct ndn_TlvDecoder *decoder)
{
  ndn_Error error;
  size_t endOffset;
  int gotExpectedType;

  if ((error = ndn_TlvDecoder_readNestedTlvsStart(decoder, ndn_Tlv_Interest, &endOffset)))
    return error;

  if ((error = ndn_decodeTlvName
       (&interest->name, signedPortionBeginOffset, signedPortionEndOffset,
        decoder)))
    return error;

  if ((error = ndn_TlvDecoder_peekType(decoder, ndn_Tlv_Selectors, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    if ((error = decodeSelectors(interest, decoder)))
      return error;
  }
  else {
    // Set selectors to none.
    interest->minSuffixComponents = -1;
    interest->maxSuffixComponents = -1;
    interest->exclude.nEntries = 0;
    interest->childSelector = -1;
    interest->mustBeFresh = 1;
  }

  // Require a Nonce, but don't force it to be 4 bytes.
  if ((error = ndn_TlvDecoder_readBlobTlv(decoder, ndn_Tlv_Nonce, &interest->nonce)))
    return error;

  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlvAsDouble
       (decoder, ndn_Tlv_InterestLifetime, endOffset, &interest->interestLifetimeMilliseconds)))
    return error;

  if ((error = ndn_TlvDecoder_peekType(decoder, ndn_Tlv_Data, endOffset, &gotExpectedType)))
    return error;
  if (gotExpectedType) {
    // Get the bytes of the Link TLV.
    size_t linkBeginOffset = decoder->offset;
    size_t linkEndOffset;
    if ((error = ndn_TlvDecoder_readNestedTlvsStart
         (decoder, ndn_Tlv_Data, &linkEndOffset)))
      return error;
    ndn_TlvDecoder_seek(decoder, linkEndOffset);

    if ((error = ndn_TlvDecoder_getSlice
         (decoder, linkBeginOffset, linkEndOffset, &interest->linkWireEncoding)))
      return error;
  }
  else
    ndn_Blob_initialize(&interest->linkWireEncoding, 0, 0);
  if ((error = ndn_TlvDecoder_readOptionalNonNegativeIntegerTlv
       (decoder, ndn_Tlv_SelectedDelegation, endOffset,
        &interest->selectedDelegationIndex)))
    return error;
  if (interest->selectedDelegationIndex >= 0 && !interest->linkWireEncoding.value)
    return NDN_ERROR_Interest_has_a_selected_delegation_but_no_link_object;

  if ((error = ndn_TlvDecoder_finishNestedTlvs(decoder, endOffset)))
    return error;

  return NDN_ERROR_success;
}
