#include "face.hpp"
#include "logger.hpp"
#include "../security/hmac-key.hpp"

#include "../ndn-cpp/c/encoding/tlv/tlv.h"
#include "../ndn-cpp/c/encoding/tlv/tlv-encoder.h"
#include "../ndn-cpp/lite/encoding/tlv-0_2-wire-format-lite.hpp"

#define FACE_DBG(...) DBG(Face, __VA_ARGS__)

namespace ndn {

Face::Face(Transport& transport)
  : m_transport(transport)
  , m_interestCb(nullptr)
  , m_interestCbArg(nullptr)
  , m_dataCb(nullptr)
  , m_dataCbArg(nullptr)
  , m_outArr(m_outBuf, NDNFACE_OUTBUF_SIZE, nullptr)
  , m_sigInfoArr(m_sigInfoBuf, NDNFACE_SIGINFOBUF_SIZE, nullptr)
  , m_sigBuf(nullptr)
  , m_signingKey(nullptr)
  , m_ownsSigningKey(false)
  , m_thisInterest(nullptr)
{
}

Face::~Face()
{
  if (m_ownsSigningKey) {
    delete m_signingKey;
  }
  if (m_sigBuf != nullptr) {
    free(m_sigBuf);
  }
}

void
Face::onInterest(InterestCallback cb, void* cbarg)
{
  m_interestCb = cb;
  m_interestCbArg = cbarg;
}

void
Face::onData(DataCallback cb, void* cbarg)
{
  m_dataCb = cb;
  m_dataCbArg = cbarg;
}

void
Face::onNack(NackCallback cb, void* cbarg)
{
  m_nackCb = cb;
  m_nackCbArg = cbarg;
}

void
Face::setSigningKey(const PrivateKey& pvtkey)
{
  bool needNewSigBuf = m_signingKey == nullptr || pvtkey.getMaxSigLength() > m_signingKey->getMaxSigLength();
  if (needNewSigBuf && m_sigBuf != nullptr) {
    free(m_sigBuf);
  }

  if (m_ownsSigningKey) {
    delete m_signingKey;
  }
  m_signingKey = &pvtkey;

  if (needNewSigBuf) {
    m_sigBuf = reinterpret_cast<uint8_t*>(malloc(pvtkey.getMaxSigLength()));
  }
}

void
Face::setHmacKey(const uint8_t* key, size_t keySize)
{
  auto hmacKey = new HmacKey(key, keySize);
  this->setSigningKey(*hmacKey);
}

void
Face::loop()
{
  uint64_t endpointId;

  int packetLimit = NDNFACE_RECEIVE_MAX;
  size_t pktSize;
  while (--packetLimit >= 0 && (pktSize = m_transport.receive(m_inBuf, NDNFACE_INBUF_SIZE, &endpointId)) > 0) {
    m_inNetPkt = m_inBuf;
    this->processPacket(pktSize, endpointId);
    yield();
  }
}

void
Face::processPacket(size_t len, uint64_t endpointId)
{
  switch (m_inNetPkt[0]) {
    case ndn_Tlv_Interest:
      this->processInterestOrNack(len, endpointId, nullptr);
      break;
    case ndn_Tlv_Data:
      this->processData(len, endpointId);
      break;
    case ndn_Tlv_LpPacket_LpPacket:
      if (m_inNetPkt == m_inBuf) {
        this->processLpPacket(len, endpointId);
        break;
      }
      // LpPacket nested in LpPacket, fallthrough to failure
    default:
      FACE_DBG(F("received unknown TLV-TYPE: 0x") << _HEX(m_inNetPkt[0]));
      break;
  }
}

void
Face::processInterestOrNack(size_t len, uint64_t endpointId, const NetworkNackLite* nack)
{
  if (m_interestCb == nullptr && nack == nullptr) {
    FACE_DBG(F("received Interest, no handler"));
    return;
  }

  ndn_NameComponent nameComps[NDNFACE_NAMECOMPS_MAX];
  ndn_ExcludeEntry excludeEntries[NDNFACE_EXCLUDE_MAX];
  ndn_NameComponent keyNameComps[NDNFACE_KEYNAMECOMPS_MAX];
  InterestLite interest(nameComps, NDNFACE_NAMECOMPS_MAX, excludeEntries, NDNFACE_EXCLUDE_MAX, keyNameComps, NDNFACE_KEYNAMECOMPS_MAX);
  ndn_Error error = Tlv0_2WireFormatLite::decodeInterest(interest, m_inNetPkt, len, &m_signedBegin, &m_signedEnd);
  if (error) {
    FACE_DBG(F("received Interest decoding error: ") << _DEC(error));
    return;
  }

  if (nack == nullptr) {
    m_thisInterest = &interest;
    m_interestCb(m_interestCbArg, interest, endpointId);
    m_thisInterest = nullptr;
  }
  else {
    m_nackCb(m_nackCbArg, *nack, interest, endpointId);
  }
}

bool
Face::verifyInterest(const PublicKey& pubkey) const
{
  if (m_thisInterest == nullptr || m_inNetPkt[0] != ndn_Tlv_Interest) {
    return false;
  }

  NameLite& name = m_thisInterest->getName();
  if (name.size() < 2) {
    return false;
  }

  const ndn::BlobLite& signatureBits = name.get(-1).getValue();
  if (signatureBits.isNull()) {
    return false;
  }

  return pubkey.verify(m_inNetPkt + m_signedBegin, m_signedEnd - m_signedBegin,
                       signatureBits.buf(), signatureBits.size());
}

void
Face::processData(size_t len, uint64_t endpointId)
{
  if (m_dataCb == nullptr) {
    FACE_DBG(F("received Data, no handler"));
    return;
  }

  ndn_NameComponent nameComps[NDNFACE_NAMECOMPS_MAX];
  ndn_NameComponent keyNameComps[NDNFACE_KEYNAMECOMPS_MAX];
  DataLite data(nameComps, NDNFACE_NAMECOMPS_MAX, keyNameComps, NDNFACE_KEYNAMECOMPS_MAX);
  m_thisData = &data;
  ndn_Error error = Tlv0_2WireFormatLite::decodeData(data, m_inNetPkt, len, &m_signedBegin, &m_signedEnd);
  if (error) {
    FACE_DBG(F("received Data decoding error: ") << _DEC(error));
    return;
  }

  m_dataCb(m_dataCbArg, data, endpointId);
  m_thisData = nullptr;
}

bool
Face::verifyData(const PublicKey& pubkey) const
{
  if (m_thisData == nullptr || m_inNetPkt[0] != ndn_Tlv_Data) {
    return false;
  }

  const ndn::BlobLite& signatureBits = m_thisData->getSignature().getSignature();
  if (signatureBits.isNull()) {
    return false;
  }

  return pubkey.verify(m_inNetPkt + m_signedBegin, m_signedEnd - m_signedBegin,
                       signatureBits.buf(), signatureBits.size());
}

void
Face::processLpPacket(size_t len, uint64_t endpointId)
{
  // Tlv0_2WireFormatLite::decodeLpPacket only recognizes Nack and IncomingFaceId;
  // the latter never appears on a non-local connection.
  ndn_LpPacketHeaderField lpHeaders[1];
  LpPacketLite lpPkt(lpHeaders, 1);
  ndn_Error error = Tlv0_2WireFormatLite::decodeLpPacket(lpPkt, m_inBuf, len);
  if (error) {
    FACE_DBG(F("received LpPacket decoding error: ") << _DEC(error));
    return;
  }

  // Tlv0_2WireFormatLite::decodeLpPacket does not recognize fragmentation headers and would drop them,
  // so the 'fragment' should be whole packets.
  const BlobLite& fragment = lpPkt.getFragmentWireEncoding();
  m_inNetPkt = fragment.buf();

  const NetworkNackLite* nack = NetworkNackLite::getFirstHeader(lpPkt);
  if (nack == nullptr) {
    this->processPacket(fragment.size(), endpointId);
    return;
  }

  if (m_nackCb == nullptr) {
    FACE_DBG(F("received Nack, no handler"));
    return;
  }
  this->processInterestOrNack(fragment.size(), endpointId, nack);
}

ndn_Error
Face::sendPacket(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  return m_transport.send(pkt, len, endpointId);
}

ndn_Error
Face::sendInterest(const InterestLite& interest, uint64_t endpointId)
{
  this->sendInterestImpl(const_cast<InterestLite&>(interest), endpointId, false);
}

ndn_Error
Face::sendSignedInterest(InterestLite& interest, uint64_t endpointId)
{
  if (m_signingKey == nullptr) {
    FACE_DBG(F("cannot sign Interest: signing key is unset"));
    return NDN_ERROR_Incorrect_key_size;
  }

  ndn_NameComponent keyNameComps[NDNFACE_KEYNAMECOMPS_MAX];
  SignatureLite signature(keyNameComps, NDNFACE_KEYNAMECOMPS_MAX);
  ndn_Error error = m_signingKey->setSignatureInfo(signature);
  if (error) {
    FACE_DBG(F("setSignatureInfo error: ") << _DEC(error));
    return error;
  }

  size_t len;
  error = Tlv0_2WireFormatLite::encodeSignatureInfo(signature, m_sigInfoArr, &len);
  if (error) {
    FACE_DBG(F("SignatureInfo encoding error: ") << _DEC(error));
    return error;
  }

  interest.getName().append(m_sigInfoBuf, len);
  error = interest.getName().append(nullptr, 0);
  if (error) {
    FACE_DBG(F("Signature appending error: ") << _DEC(error));
    return error;
  }

  return this->sendInterestImpl(interest, endpointId, true);
}

ndn_Error
Face::sendInterestImpl(InterestLite& interest, uint64_t endpointId, bool needSigning)
{
  size_t signedBegin, signedEnd, len;
  ndn_Error error = Tlv0_2WireFormatLite::encodeInterest(interest, &signedBegin, &signedEnd, m_outArr, &len);
  if (error) {
    FACE_DBG(F("send Interest encoding-1 error: ") << _DEC(error));
    return error;
  }

  if (needSigning) {
    int sigLen = m_signingKey->sign(m_outBuf + signedBegin, signedEnd - signedBegin, m_sigBuf);
    if (sigLen == 0) {
      FACE_DBG(F("signing error"));
      return NDN_ERROR_Error_in_sign_operation;
    }
    NameLite& name = interest.getName();
    name.pop();
    name.append(m_sigBuf, sigLen);
    error = Tlv0_2WireFormatLite::encodeInterest(interest, &signedBegin, &signedEnd, m_outArr, &len);
    if (error) {
      FACE_DBG(F("send Interest encoding-2 error: ") << _DEC(error));
      return error;
    }
  }

  return this->sendPacket(m_outBuf, len, endpointId);
}

ndn_Error
Face::sendData(DataLite& data, uint64_t endpointId)
{
  if (m_signingKey == nullptr) {
    FACE_DBG(F("cannot sign Data: signing key is unset"));
    return NDN_ERROR_Incorrect_key_size;
  }
  ndn_Error error = m_signingKey->setSignatureInfo(data.getSignature());
  if (error) {
    FACE_DBG(F("setSignatureInfo error: ") << _DEC(error));
    return error;
  }

  size_t signedBegin, signedEnd, len;
  error = Tlv0_2WireFormatLite::encodeData(data, &signedBegin, &signedEnd, m_outArr, &len);
  if (error) {
    FACE_DBG(F("send Data encoding-1 error: ") << _DEC(error));
    return error;
  }

  int sigLen = m_signingKey->sign(m_outBuf + m_signedBegin, m_signedEnd - m_signedBegin, m_sigBuf);
  if (sigLen == 0) {
    FACE_DBG(F("signing error"));
    return NDN_ERROR_Error_in_sign_operation;
  }
  data.getSignature().setSignature(BlobLite(m_sigBuf, sigLen));
  error = Tlv0_2WireFormatLite::encodeData(data, &signedBegin, &signedEnd, m_outArr, &len);
  if (error) {
    FACE_DBG(F("send Data encoding-2 error: ") << _DEC(error));
    return error;
  }

  return this->sendPacket(m_outBuf, len, endpointId);
}

ndn_Error
Face::sendNack(const NetworkNackLite& nack, const InterestLite& interest, uint64_t endpointId)
{
  DynamicUInt8ArrayLite outArr(m_outBuf + NDNFACE_OUTNACK_HEADROOM, NDNFACE_OUTBUF_SIZE - NDNFACE_OUTNACK_HEADROOM, nullptr);
  size_t signedBegin, signedEnd, interestSize;
  ndn_Error error = Tlv0_2WireFormatLite::encodeInterest(interest, &signedBegin, &signedEnd, outArr, &interestSize);
  if (error) {
    FACE_DBG(F("send Nack encoding error: ") << _DEC(error));
    return error;
  }

  static const uint8_t NACK_HDR[] PROGMEM {0xFD, 0x03, 0x20, 0x05, 0xFD, 0x03, 0x21, 0x01, 0xFF, 0x50};
  const int NACK_REASON_OFFSET = 8; // where is NackReason TLV-VALUE

  size_t fragmentLenSize = ndn_TlvEncoder_sizeOfVarNumber(interestSize);
  size_t lpPacketLen = sizeof(NACK_HDR) + fragmentLenSize + interestSize;
  size_t lpPacketLenSize = ndn_TlvEncoder_sizeOfVarNumber(lpPacketLen);
  size_t lpPacketSize = 1 + lpPacketLenSize + lpPacketLen;
  size_t lpHeaderSize = lpPacketSize - interestSize;

  if (lpHeaderSize > NDNFACE_OUTNACK_HEADROOM) {
    FACE_DBG(F("insufficient headroom"));
    return NDN_ERROR_cannot_store_more_header_bytes_than_the_size_of_headerBuffer;
  }

  ndn_TlvEncoder encoder;
  ndn_TlvEncoder_initialize(&encoder, reinterpret_cast<ndn_DynamicUInt8Array*>(&m_outArr));
  ndn_TlvEncoder_seek(&encoder, NDNFACE_OUTNACK_HEADROOM - lpHeaderSize);
  ndn_TlvEncoder_writeTypeAndLength(&encoder, ndn_Tlv_LpPacket_LpPacket, lpPacketLen);
  memcpy_P(m_outBuf + encoder.offset, NACK_HDR, sizeof(NACK_HDR));
  m_outBuf[encoder.offset + NACK_REASON_OFFSET] = static_cast<uint8_t>(nack.getReason());
  encoder.offset += sizeof(NACK_HDR);
  ndn_TlvEncoder_writeVarNumber(&encoder, interestSize);

  return this->sendPacket(m_outBuf + NDNFACE_OUTNACK_HEADROOM - lpHeaderSize, lpPacketSize, endpointId);
}

} // namespace ndn
