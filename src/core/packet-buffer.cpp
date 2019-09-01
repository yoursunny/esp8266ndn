#include "packet-buffer.hpp"
#include "../security/public-key.hpp"

#include "../ndn-cpp/c/data.h"
#include "../ndn-cpp/c/interest.h"
#include "../ndn-cpp/c/network-nack.h"
#include "../ndn-cpp/c/encoding/tlv/tlv.h"
#include "../ndn-cpp/c/encoding/tlv/tlv-decoder.h"
#include "../ndn-cpp/lite/encoding/tlv-0_2-wire-format-lite.hpp"

namespace ndn {

PacketBuffer::PacketBuffer(const Options& options)
  : endpointId(0)
  , m_buf(nullptr)
  , m_netPkt(nullptr)
  , m_nameComps(nullptr)
  , m_keyNameComps(nullptr)
  , m_maxSize(options.maxSize)
  , m_maxNameComps(options.maxNameComps)
  , m_maxKeyNameComps(options.maxKeyNameComps)
  , m_netPktLen(0)
  , m_signedBegin(0)
  , m_signedEnd(0)
{
  size_t allocSize = sizeof(ndn_NameComponent) * (m_maxNameComps + m_maxKeyNameComps) + m_maxSize;
  m_nameComps = reinterpret_cast<ndn_NameComponent*>(new uint8_t[allocSize]);
  m_keyNameComps = &m_nameComps[m_maxNameComps];
  m_buf = reinterpret_cast<uint8_t*>(&m_keyNameComps[m_maxKeyNameComps]);
}

PacketBuffer::~PacketBuffer()
{
  delete[] reinterpret_cast<uint8_t*>(m_nameComps);
}

std::tuple<uint8_t*, size_t>
PacketBuffer::useBuffer()
{
  endpointId = 0;
  m_netPkt = nullptr;
  m_netPktLen = 0;
  m_signedBegin = 0;
  m_signedEnd = 0;
  return std::make_tuple(m_buf, m_maxSize);
}

ndn_Error
PacketBuffer::parse(size_t len)
{
  m_netPkt = m_buf;
  m_netPktLen = len;
  return this->parsePacket();
}

ndn_Error
PacketBuffer::parsePacket()
{
  if (m_netPktLen < 1) {
    return NDN_ERROR_read_past_the_end_of_the_input;
  }
  switch (m_netPkt[0]) {
    case ndn_Tlv_Interest:
      return this->parseInterestOrNack(nullptr);
    case ndn_Tlv_Data:
      return this->parseData();
    case ndn_Tlv_LpPacket_LpPacket:
      if (m_netPkt == m_buf) {
        return this->parseLpPacket();
      }
      // LpPacket nested in LpPacket, fallthrough to failure
    default:
      return NDN_ERROR_header_type_is_out_of_range;
  }
}

ndn_Error
PacketBuffer::parseInterestOrNack(const NetworkNackLite* nack)
{
  if (nack == nullptr) {
    ndn_NetworkNack_initialize(&m_nack);
  }
  else {
    m_nack = *reinterpret_cast<const ndn_NetworkNack*>(nack);
  }

  ndn_Interest_initialize(&m_interest, m_nameComps, m_maxNameComps, nullptr, 0, m_keyNameComps, m_maxKeyNameComps);
  InterestLite& interest = InterestLite::downCast(m_interest);
  size_t signedBegin = 0, signedEnd = 0;
  ndn_Error e = Tlv0_2WireFormatLite::decodeInterest(interest, m_netPkt, m_netPktLen, &signedBegin, &signedEnd);
  m_signedBegin = static_cast<uint16_t>(signedBegin);
  m_signedEnd = static_cast<uint16_t>(signedEnd);
  return e;
}

ndn_Error
PacketBuffer::parseData()
{
  ndn_Data_initialize(&m_data, m_nameComps, m_maxNameComps, m_keyNameComps, m_maxKeyNameComps);
  DataLite& data = DataLite::downCast(m_data);
  size_t signedBegin = 0, signedEnd = 0;
  ndn_Error e = Tlv0_2WireFormatLite::decodeData(data, m_netPkt, m_netPktLen, &signedBegin, &signedEnd);
  m_signedBegin = static_cast<uint16_t>(signedBegin);
  m_signedEnd = static_cast<uint16_t>(signedEnd);
  return e;
}

ndn_Error
PacketBuffer::parseLpPacket()
{
  // Tlv0_2WireFormatLite::decodeLpPacket only recognizes Nack and IncomingFaceId;
  // the latter never appears on a non-local connection.
  ndn_LpPacketHeaderField lpHeaders[1];
  LpPacketLite lpPkt(lpHeaders, 1);
  ndn_Error error = Tlv0_2WireFormatLite::decodeLpPacket(lpPkt, m_netPkt, m_netPktLen);
  if (error) {
    return error;
  }

  // Tlv0_2WireFormatLite::decodeLpPacket does not recognize fragmentation headers and would drop them,
  // so the 'fragment' should be whole packets.
  const BlobLite& fragment = lpPkt.getFragmentWireEncoding();
  m_netPkt = fragment.buf();
  m_netPktLen = fragment.size();

  const NetworkNackLite* nack = NetworkNackLite::getFirstHeader(lpPkt);
  if (nack == nullptr) {
    return this->parsePacket();
  }
  return this->parseInterestOrNack(nack);
}

PacketType
PacketBuffer::getPktType() const
{
  if (m_netPkt == nullptr) {
    return PacketType::NONE;
  }
  if (m_netPkt[0] == ndn_Tlv_Data) {
    return PacketType::DATA;
  }
  if (m_netPkt[0] == ndn_Tlv_Interest) {
    if (m_nack.reason != ndn_NetworkNackReason_NONE) {
      return PacketType::NACK;
    }
    return PacketType::INTEREST;
  }
  return PacketType::NONE;
}

std::tuple<const uint8_t*, size_t>
PacketBuffer::getRaw(RawLayer layer) const
{
  if (layer == RAW_L2) {
    return std::make_tuple(m_buf, (m_netPkt - m_buf) + m_netPktLen);
  }
  return std::make_tuple(m_netPkt, m_netPktLen);
}

const InterestLite*
PacketBuffer::getInterest() const
{
  PacketType pktType = this->getPktType();
  if (pktType == PacketType::INTEREST ||
      pktType == PacketType::NACK) {
    return &InterestLite::downCast(m_interest);
  }
  return nullptr;
}

const DataLite*
PacketBuffer::getData() const
{
  if (this->getPktType() == PacketType::DATA) {
    return &DataLite::downCast(m_data);
  }
  return nullptr;
}

const NetworkNackLite*
PacketBuffer::getNack() const
{
  if (this->getPktType() == PacketType::NACK) {
    return &NetworkNackLite::downCast(m_nack);
  }
  return nullptr;
}

PacketBuffer::VerifyResult
PacketBuffer::verify(const PublicKey& pubKey) const
{
  switch (this->getPktType()) {
    case PacketType::INTEREST:
      return this->verifyInterest(pubKey);
    case PacketType::DATA:
      return this->verifyData(pubKey);
    default:
      return VERIFY_NO_PKT;
  }
}

PacketBuffer::VerifyResult
PacketBuffer::verifyInterest(const PublicKey& pubKey) const
{
  const NameLite& name = this->getInterest()->getName();
  if (name.size() <= 2) {
    return VERIFY_PARSE_ERR;
  }

  const BlobLite& sigValueComp = name.get(-1).getValue();
  ndn_TlvDecoder decoder;
  ndn_TlvDecoder_initialize(&decoder, sigValueComp.buf(), sigValueComp.size());
  ndn_Blob sigValue = {0};
  ndn_Error e = ndn_TlvDecoder_readBlobTlv(&decoder, ndn_Tlv_SignatureValue, &sigValue);
  if (e) {
    return VERIFY_PARSE_ERR;
  }

  return this->verifySig(pubKey, BlobLite::downCast(sigValue));
}

PacketBuffer::VerifyResult
PacketBuffer::verifyData(const PublicKey& pubKey) const
{
  return this->verifySig(pubKey, this->getData()->getSignature().getSignature());
}

PacketBuffer::VerifyResult
PacketBuffer::verifySig(const PublicKey& pubKey, const BlobLite& sig) const
{
  if (sig.isNull()) {
    return VERIFY_PARSE_ERR;
  }

  bool res = pubKey.verify(m_buf + m_signedBegin, m_signedEnd - m_signedBegin,
                           sig.buf(), sig.size());
  return res ? VERIFY_OK : VERIFY_BAD_SIG;
}

} // namespace ndn
