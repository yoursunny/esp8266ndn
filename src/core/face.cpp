#include "face.hpp"
#include "logger.hpp"
#include "../security/hmac-key.hpp"

#include "../ndn-cpp/c/encoding/tlv/tlv.h"
#include "../ndn-cpp/lite/encoding/tlv-0_2-wire-format-lite.hpp"

#define FACE_DBG(...) DBG(Face, __VA_ARGS__)

namespace ndn {

Face::Face(Transport& transport)
  : m_transport(transport)
  , m_interestCb(nullptr)
  , m_interestCbArg(nullptr)
  , m_dataCb(nullptr)
  , m_dataCbArg(nullptr)
{
}

Face::~Face() = default;

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
Face::setHmacKey(const uint8_t* key, size_t keySize)
{
  m_hmacKey.reset(new HmacKey(key, keySize));
}

void
Face::loop()
{
  uint8_t* inBuf = reinterpret_cast<uint8_t*>(malloc(NDNFACE_INBUF_SIZE));
  if (inBuf == nullptr) {
    FACE_DBG(F("cannot allocate inBuf"));
  }
  uint64_t endpointId;

  int packetLimit = NDNFACE_RECEIVE_MAX;
  size_t pktSize;
  while (--packetLimit >= 0 && (pktSize = m_transport.receive(inBuf, NDNFACE_INBUF_SIZE, &endpointId)) > 0) {
    this->processPacket(inBuf, pktSize, endpointId);
    yield();
  }
  free(inBuf);
}

void
Face::processPacket(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  switch (pkt[0]) {
    case ndn_Tlv_Interest:
      this->processInterest(pkt, len, endpointId);
      break;
    case ndn_Tlv_Data:
      this->processData(pkt, len, endpointId);
      break;
    default:
      FACE_DBG(F("received unknown TLV-TYPE: 0x") << _HEX(pkt[0]));
      break;
  }
}

void
Face::processInterest(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  if (m_interestCb == nullptr) {
    FACE_DBG(F("received Interest, no handler"));
    return;
  }

  ndn_NameComponent nameComps[NDNFACE_NAMECOMPS_MAX];
  ndn_ExcludeEntry excludeEntries[NDNFACE_EXCLUDE_MAX];
  ndn_NameComponent keyNameComps[NDNFACE_KEYNAMECOMPS_MAX];
  InterestLite interest(nameComps, NDNFACE_NAMECOMPS_MAX, excludeEntries, NDNFACE_EXCLUDE_MAX, keyNameComps, NDNFACE_KEYNAMECOMPS_MAX);
  size_t signedBegin, signedEnd;
  ndn_Error error = Tlv0_2WireFormatLite::decodeInterest(interest, pkt, len, &signedBegin, &signedEnd);
  if (error) {
    FACE_DBG(F("received Interest decoding error: ") << _DEC(error));
    return;
  }

  m_interestCb(m_interestCbArg, interest, endpointId);
}

void
Face::processData(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  if (m_dataCb == nullptr) {
    FACE_DBG(F("received Data, no handler"));
    return;
  }

  ndn_NameComponent nameComps[NDNFACE_NAMECOMPS_MAX];
  ndn_NameComponent keyNameComps[NDNFACE_KEYNAMECOMPS_MAX];
  DataLite data(nameComps, NDNFACE_NAMECOMPS_MAX, keyNameComps, NDNFACE_KEYNAMECOMPS_MAX);
  size_t signedBegin, signedEnd;
  ndn_Error error = Tlv0_2WireFormatLite::decodeData(data, pkt, len, &signedBegin, &signedEnd);
  if (error) {
    FACE_DBG(F("received Data decoding error: ") << _DEC(error));
    return;
  }

  m_dataCb(m_dataCbArg, data, endpointId);
}

void
Face::sendPacket(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  m_transport.send(pkt, len, endpointId);
}

void
Face::sendInterest(InterestLite& interest, uint64_t endpointId)
{
  uint8_t* outBuf = reinterpret_cast<uint8_t*>(malloc(NDNFACE_OUTBUF_SIZE));
  if (outBuf == nullptr) {
    FACE_DBG(F("cannot allocate outBuf"));
  }
  DynamicUInt8ArrayLite output(outBuf, NDNFACE_OUTBUF_SIZE, nullptr);
  size_t signedBegin, signedEnd, len;
  ndn_Error error = Tlv0_2WireFormatLite::encodeInterest(interest, &signedBegin, &signedEnd, output, &len);
  if (error) {
    FACE_DBG(F("send Interest encoding error: ") << _DEC(error));
    return;
  }

  this->sendPacket(outBuf, len, endpointId);
  free(outBuf);
}

void
Face::sendData(DataLite& data, uint64_t endpointId)
{
  if (m_hmacKey == nullptr) {
    FACE_DBG(F("cannot sign Data: HMAC key is unset"));
    return;
  }
  m_hmacKey->setSignatureInfo(data.getSignature());

  uint8_t* outBuf = reinterpret_cast<uint8_t*>(malloc(NDNFACE_OUTBUF_SIZE));
  if (outBuf == nullptr) {
    FACE_DBG(F("cannot allocate outBuf"));
    return;
  }
  DynamicUInt8ArrayLite output(outBuf, NDNFACE_OUTBUF_SIZE, nullptr);
  size_t signedBegin, signedEnd, len;
  ndn_Error error = Tlv0_2WireFormatLite::encodeData(data, &signedBegin, &signedEnd, output, &len);
  if (error) {
    FACE_DBG(F("send Data encoding error: ") << _DEC(error));
    free(outBuf);
    return;
  }

  uint8_t signatureValue[ndn_SHA256_DIGEST_SIZE];
  m_hmacKey->sign(outBuf + signedBegin, signedEnd - signedBegin, signatureValue);
  data.getSignature().setSignature(BlobLite(signatureValue, ndn_SHA256_DIGEST_SIZE));
  error = Tlv0_2WireFormatLite::encodeData(data, &signedBegin, &signedEnd, output, &len);
  if (error) {
    FACE_DBG(F("send Data encoding error: ") << _DEC(error));
    free(outBuf);
    return;
  }

  this->sendPacket(outBuf, len, endpointId);
  free(outBuf);
}

} // namespace ndn
