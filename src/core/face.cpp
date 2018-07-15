#include "face.hpp"
#include "logger.hpp"
#include "../security/private-key.hpp"
#include "../transport/transport.hpp"

#include "../ndn-cpp/c/encoding/tlv/tlv.h"
#include "../ndn-cpp/c/encoding/tlv/tlv-encoder.h"
#include "../ndn-cpp/lite/encoding/tlv-0_2-wire-format-lite.hpp"

#define FACE_DBG(...) DBG(Face, __VA_ARGS__)

namespace ndn {

Face::Face(Transport& transport)
  : m_transport(transport)
  , m_pb(new PacketBuffer({}))
  , m_interestCb(nullptr)
  , m_interestCbArg(nullptr)
  , m_dataCb(nullptr)
  , m_dataCbArg(nullptr)
  , m_outArr(m_outBuf, NDNFACE_OUTBUF_SIZE, nullptr)
  , m_sigInfoArr(m_sigInfoBuf, NDNFACE_SIGINFOBUF_SIZE, nullptr)
  , m_sigBuf(nullptr)
  , m_signingKey(nullptr)
{
}

Face::~Face()
{
  if (m_sigBuf != nullptr) {
    free(m_sigBuf);
  }
  delete m_pb;
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

  m_signingKey = &pvtkey;

  if (needNewSigBuf) {
    m_sigBuf = reinterpret_cast<uint8_t*>(malloc(2 + pvtkey.getMaxSigLength()));
  }
}

void
Face::loop(int packetLimit)
{
  while (--packetLimit >= 0) {
    uint64_t endpointId;
    ndn_Error e = this->receive(m_pb, &endpointId);
    if (e) {
      FACE_DBG(F("receive error ") << _DEC(e));
    }
    else {
      switch (m_pb->getPacketType()) {
        case 0:
          return; // no more packets
        case ndn_Tlv_Interest:
          if (m_interestCb == nullptr) {
            FACE_DBG(F("received Interest, no handler"));
          }
          else {
            m_interestCb(m_interestCbArg, *m_pb->getInterest(), endpointId);
          }
          break;
        case ndn_Tlv_Data:
          if (m_dataCb == nullptr) {
            FACE_DBG(F("received Data, no handler"));
          }
          else {
            m_dataCb(m_dataCbArg, *m_pb->getData(), endpointId);
          }
          break;
        case ndn_Tlv_LpPacket_Nack:
          if (m_nackCb == nullptr) {
            FACE_DBG(F("received Nack, no handler"));
          }
          else {
            m_nackCb(m_nackCbArg, *m_pb->getNack(), *m_pb->getInterest(), endpointId);
          }
          break;
      }
    }
    yield();
  }
}

ndn_Error
Face::receive(PacketBuffer* pb, uint64_t* endpointId)
{
  uint8_t* buf;
  size_t bufSize;
  std::tie(buf, bufSize) = pb->useBuffer();

  uint64_t ignoredEndpointId;
  if (endpointId == nullptr) {
    endpointId = &ignoredEndpointId;
  }

  size_t pktSize = m_transport.receive(buf, bufSize, endpointId);
  if (pktSize == 0) {
    return NDN_ERROR_success;
  }

  return pb->parse(pktSize);
}

bool
Face::verifyInterest(const PublicKey& pubKey) const
{
  if (m_pb->getPacketType() != ndn_Tlv_Interest) {
    FACE_DBG(F("last packet is not Interest"));
    return false;
  }

  PacketBuffer::VerifyResult res = m_pb->verify(pubKey);
  if (res) {
    FACE_DBG(F("Interest verify error ") << _DEC(res));
  }
  return res == PacketBuffer::VERIFY_OK;
}

bool
Face::verifyData(const PublicKey& pubKey) const
{
  if (m_pb->getPacketType() != ndn_Tlv_Data) {
    FACE_DBG(F("last packet is not Data"));
    return false;
  }

  PacketBuffer::VerifyResult res = m_pb->verify(pubKey);
  if (res) {
    FACE_DBG(F("Data verify error ") << _DEC(res));
  }
  return res == PacketBuffer::VERIFY_OK;
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
    int sigLen = m_signingKey->sign(m_outBuf + signedBegin, signedEnd - signedBegin, m_sigBuf + 2);
    if (sigLen == 0 || sigLen > 253) {
      FACE_DBG(F("signing error"));
      return NDN_ERROR_Error_in_sign_operation;
    }
    m_sigBuf[0] = ndn_Tlv_SignatureValue;
    m_sigBuf[1] = sigLen;
    sigLen += 2;

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

  int sigLen = m_signingKey->sign(m_outBuf + signedBegin, signedEnd - signedBegin, m_sigBuf);
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
