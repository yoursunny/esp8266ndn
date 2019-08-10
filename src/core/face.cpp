#include "face.hpp"
#include "logger.hpp"
#include "uri.hpp"
#include "with-components-buffer.hpp"
#include "../security/private-key.hpp"
#include "../transport/transport.hpp"

#include "../ndn-cpp/c/encoding/tlv/tlv.h"
#include "../ndn-cpp/c/encoding/tlv/tlv-encoder.h"
#include "../ndn-cpp/lite/encoding/tlv-0_2-wire-format-lite.hpp"

#define LOG(...) LOGGER(Face, __VA_ARGS__)

namespace ndn {

class Face::TracingHandler : public PacketHandler
{
public:
  TracingHandler(Face& face, Print& output, const String& prefix)
    : m_output(output)
    , m_prefix(prefix.length() == 0 ? " " : " " + prefix + " ")
  {
    face.addHandler(this, -127);
  }

  void
  logInterest(const InterestLite& interest, uint64_t endpointId, char dir = '<')
  {
    m_output << millis() << m_prefix << dir << "I " << PrintUri{interest.getName()}
             << F(" endpoint=") << _HEX(endpointId) << endl;
  }

  void
  logData(const DataLite& data, uint64_t endpointId, char dir = '<')
  {
    m_output << millis() << m_prefix << dir << "D " << PrintUri{data.getName()}
             << F(" endpoint=") << _HEX(endpointId) << endl;
  }

  void
  logNack(const NetworkNackLite& nackHeader, const InterestLite& interest,
          uint64_t endpointId, char dir = '<')
  {
    m_output << millis() << m_prefix << dir << "N " << PrintUri{interest.getName()}
             << '~' << static_cast<int>(nackHeader.getReason())
             << F(" endpoint=") << _HEX(endpointId) << endl;
  }

  bool
  processInterest(const InterestLite& interest, uint64_t endpointId) override
  {
    this->logInterest(interest, endpointId, '>');
    return false;
  }

  bool
  processData(const DataLite& data, uint64_t endpointId) override
  {
    this->logData(data, endpointId, '>');
    return false;
  }

  bool
  processNack(const NetworkNackLite& nackHeader, const InterestLite& interest,
              uint64_t endpointId) override
  {
    this->logNack(nackHeader, interest, endpointId, '>');
    return false;
  }

private:
  Print& m_output;
  String m_prefix;
};

Face::Face(Transport& transport)
  : m_transport(transport)
  , m_handler(nullptr)
  , m_wantNack(true)
  , m_addedReceiveBuffers(false)
  , m_pb(nullptr)
  , m_outArr(m_outBuf, NDNFACE_OUTBUF_SIZE, nullptr)
  , m_sigInfoArr(m_sigInfoBuf, NDNFACE_SIGINFOBUF_SIZE, nullptr)
  , m_sigBuf(0)
  , m_signingKey(nullptr)
{
  m_transport.onReceive(&Face::transportReceive, this);
}

Face::~Face()
{
  if (m_pb != nullptr) {
    delete m_pb;
  }
}

bool
Face::addHandler(PacketHandler* h, int8_t prio)
{
  if (h->m_face != nullptr) {
    return false;
  }
  h->m_face = this;
  h->m_prio = prio;

  PacketHandler** next = &m_handler;
  for (; *next != nullptr && (*next)->m_prio < prio; next = &(*next)->m_next) {
  }
  h->m_next = *next;
  *next = h;
  return true;
}

bool
Face::removeHandler(PacketHandler* h)
{
  if (h->m_face != this) {
    return false;
  }
  h->m_face = nullptr;

  for (PacketHandler** cur = &m_handler; *cur != nullptr; cur = &(*cur)->m_next) {
    if (*cur == h) {
      *cur = h->m_next;
      return true;
    }
  }
  return false;
}

void
Face::enableTracing(Print& output, const String& prefix)
{
  m_tracing.reset(new TracingHandler(*this, output, prefix));
}

void
Face::setSigningKey(const PrivateKey& pvtkey)
{
  m_signingKey = &pvtkey;
}

PacketBuffer*
Face::popReceiveBuffer()
{
  PacketBuffer* pb = m_pb;
  m_pb = nullptr;
  return pb;
}

void
Face::pushReceiveBuffer(PacketBuffer* pb)
{
  m_addedReceiveBuffers = m_transport.pushReceiveBuffer(pb) || m_addedReceiveBuffers;
}

int
Face::addReceiveBuffers(int count, const PacketBuffer::Options& options)
{
  int n = 0;
  while (n < count && m_transport.canPushReceiveBuffer()) {
    if (m_transport.pushReceiveBuffer(new PacketBuffer(options))) {
      m_addedReceiveBuffers = true;
      ++n;
    }
    else {
      break;
    }
  }
  return n;
}

void
Face::loop()
{
  if (m_pb != nullptr) {
    m_transport.pushReceiveBuffer(m_pb);
    m_pb = nullptr;
  }
  if (!m_addedReceiveBuffers) {
    addReceiveBuffers(1);
  }
  m_transport.loop();
}

void
Face::transportReceive(void* self, PacketBuffer* pb)
{
  static_cast<Face*>(self)->receive(pb);
}

void
Face::receive(PacketBuffer* pb)
{
  m_pb = pb;
  switch (m_pb->getPktType()) {
    case PacketType::INTEREST: {
      bool isAccepted = false;
      const InterestLite& interest = *m_pb->getInterest();
      for (PacketHandler* h = m_handler; h != nullptr && !isAccepted; h = h->m_next) {
        isAccepted = h->processInterest(interest, m_pb->endpointId);
      }
      if (!isAccepted) {
        if (m_wantNack) {
          ndn::NetworkNackLite nack;
          nack.setReason(ndn_NetworkNackReason_NO_ROUTE);
          this->sendNack(nack, interest, m_pb->endpointId);
        }
        else {
          LOG(F("received Interest, no handler"));
        }
      }
      break;
    }
    case PacketType::DATA: {
      bool isAccepted = false;
      const DataLite& data = *m_pb->getData();
      for (PacketHandler* h = m_handler; h != nullptr && !isAccepted; h = h->m_next) {
        isAccepted = h->processData(data, m_pb->endpointId);
      }
      if (!isAccepted) {
        LOG(F("received Data, no handler"));
      }
      break;
    }
    case PacketType::NACK: {
      bool isAccepted = false;
      const NetworkNackLite& nackHeader = *m_pb->getNack();
      const InterestLite& interest = *m_pb->getInterest();
      for (PacketHandler* h = m_handler; h != nullptr && !isAccepted; h = h->m_next) {
        isAccepted = h->processNack(nackHeader, interest, m_pb->endpointId);
      }
      if (!isAccepted) {
        LOG(F("received Nack, no handler"));
      }
      break;
    }
    case PacketType::NONE:
      break;
  }
}

bool
Face::verifyInterest(const PublicKey& pubKey) const
{
  if (m_pb->getPktType() != PacketType::INTEREST) {
    LOG(F("last packet is not Interest"));
    return false;
  }

  PacketBuffer::VerifyResult res = m_pb->verify(pubKey);
  if (res) {
    LOG(F("Interest verify error ") << _DEC(res));
  }
  return res == PacketBuffer::VERIFY_OK;
}

bool
Face::verifyData(const PublicKey& pubKey) const
{
  if (m_pb->getPktType() != PacketType::DATA) {
    LOG(F("last packet is not Data"));
    return false;
  }

  PacketBuffer::VerifyResult res = m_pb->verify(pubKey);
  if (res) {
    LOG(F("Data verify error ") << _DEC(res));
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
  return this->sendInterestImpl(const_cast<InterestLite&>(interest), endpointId, nullptr);
}

ndn_Error
Face::sendSignedInterest(InterestLite& interest, uint64_t endpointId, const PrivateKey* pvtkey)
{
  if (pvtkey == nullptr) {
    pvtkey = m_signingKey;
  }
  if (pvtkey == nullptr) {
    LOG(F("cannot sign Interest: signing key is unset"));
    return NDN_ERROR_Incorrect_key_size;
  }

  SignatureWCB<NDNFACE_KEYNAMECOMPS_MAX> signature;
  ndn_Error error = pvtkey->setSignatureInfo(signature);
  if (error) {
    LOG(F("setSignatureInfo error: ") << _DEC(error));
    return error;
  }

  size_t len;
  error = Tlv0_2WireFormatLite::encodeSignatureInfo(signature, m_sigInfoArr, &len);
  if (error) {
    LOG(F("SignatureInfo encoding error: ") << _DEC(error));
    return error;
  }

  interest.getName().append(m_sigInfoBuf, len);
  error = interest.getName().append(nullptr, 0);
  if (error) {
    LOG(F("Signature appending error: ") << _DEC(error));
    return error;
  }

  return this->sendInterestImpl(interest, endpointId, pvtkey);
}

ndn_Error
Face::sendInterestImpl(InterestLite& interest, uint64_t endpointId, const PrivateKey* pvtkey)
{
  size_t signedBegin, signedEnd, len;
  ndn_Error error = Tlv0_2WireFormatLite::encodeInterest(interest, &signedBegin, &signedEnd, m_outArr, &len);
  if (error) {
    LOG(F("send Interest encoding-1 error: ") << _DEC(error));
    return error;
  }

  if (pvtkey != nullptr) {
    int sigLen;
    error = this->signImpl(*pvtkey, m_outBuf + signedBegin, signedEnd - signedBegin, sigLen);
    if (error) {
      LOG(F("signing error ") << _DEC(error));
      return error;
    }

    NameLite& name = interest.getName();
    name.pop();
    name.append(m_sigBuf.getArray(), sigLen + 2);
    error = Tlv0_2WireFormatLite::encodeInterest(interest, &signedBegin, &signedEnd, m_outArr, &len);
    if (error) {
      LOG(F("send Interest encoding-2 error: ") << _DEC(error));
      return error;
    }
  }

  if (m_tracing) {
    m_tracing->logInterest(interest, endpointId);
  }
  return this->sendPacket(m_outBuf, len, endpointId);
}

ndn_Error
Face::sendData(DataLite& data, uint64_t endpointId, const PrivateKey* pvtkey)
{
  if (pvtkey == nullptr) {
    pvtkey = m_signingKey;
  }
  if (pvtkey == nullptr) {
    LOG(F("cannot sign Data: signing key is unset"));
    return NDN_ERROR_Incorrect_key_size;
  }
  ndn_Error error = pvtkey->setSignatureInfo(data.getSignature());
  if (error) {
    LOG(F("setSignatureInfo error: ") << _DEC(error));
    return error;
  }

  size_t signedBegin, signedEnd, len;
  error = Tlv0_2WireFormatLite::encodeData(data, &signedBegin, &signedEnd, m_outArr, &len);
  if (error) {
    LOG(F("send Data encoding-1 error: ") << _DEC(error));
    return error;
  }

  int sigLen;
  error = this->signImpl(*pvtkey, m_outBuf + signedBegin, signedEnd - signedBegin, sigLen);
  if (error) {
    LOG(F("signing error ") << _DEC(error));
    return error;
  }
  data.getSignature().setSignature(BlobLite(&m_sigBuf.getArray()[2], sigLen));
  error = Tlv0_2WireFormatLite::encodeData(data, &signedBegin, &signedEnd, m_outArr, &len);
  if (error) {
    LOG(F("send Data encoding-2 error: ") << _DEC(error));
    return error;
  }

  if (m_tracing) {
    m_tracing->logData(data, endpointId);
  }
  return this->sendPacket(m_outBuf, len, endpointId);
}

ndn_Error
Face::signImpl(const PrivateKey& pvtkey, const uint8_t* input, size_t inputLen, int& sigLen)
{
  if (pvtkey.getMaxSigLength() > 253) { // we expect TLV-LENGTH to be one octet
    return NDN_ERROR_TLV_length_exceeds_buffer_length;
  }
  ndn_Error error = m_sigBuf.ensureLength(2 + pvtkey.getMaxSigLength());
  if (error) {
    return error;
  }

  uint8_t* sigBuf = m_sigBuf.getArray();
  sigLen = pvtkey.sign(input, inputLen, &sigBuf[2]);
  if (sigLen == 0) {
    return NDN_ERROR_Error_in_sign_operation;
  }

  sigBuf[1] = sigLen;
  sigBuf[0] = ndn_Tlv_SignatureValue;
  return NDN_ERROR_success;
}

ndn_Error
Face::sendNack(const NetworkNackLite& nack, const InterestLite& interest, uint64_t endpointId)
{
  DynamicUInt8ArrayLite outArr(m_outBuf + NDNFACE_OUTNACK_HEADROOM, NDNFACE_OUTBUF_SIZE - NDNFACE_OUTNACK_HEADROOM, nullptr);
  size_t signedBegin, signedEnd, interestSize;
  ndn_Error error = Tlv0_2WireFormatLite::encodeInterest(interest, &signedBegin, &signedEnd, outArr, &interestSize);
  if (error) {
    LOG(F("send Nack encoding error: ") << _DEC(error));
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
    LOG(F("insufficient headroom"));
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

  if (m_tracing) {
    m_tracing->logNack(nack, interest, endpointId);
  }
  return this->sendPacket(m_outBuf + NDNFACE_OUTNACK_HEADROOM - lpHeaderSize, lpPacketSize, endpointId);
}

} // namespace ndn
