#include "lite-frag.hpp"
#include "../core/logger.hpp"

#define LITEFRAG_DBG(...) DBG(LiteFrag, __VA_ARGS__)

namespace ndn {

struct __attribute__((packed)) ReassHdr
{
  uint16_t len;
  uint8_t b0;
  uint16_t id;
  uint8_t payload[0];
};

enum {
  LiteFragHdr_b0_HB = 0x80,
  LiteFragHdr_b0_MF = 0x20,
  LiteFragHdr_b0_SEQ_MASK = 0x1F,
};

LiteFrag::LiteFrag(Transport& inner)
  : inner(inner)
  , m_rbuf(nullptr)
  , m_rbufSize(0)
  , m_offset(0)
  , m_id(0)
  , m_nextSeq(0)
{
}

LiteFrag::~LiteFrag()
{
  end();
}

void
LiteFrag::begin(size_t reassBufSize, size_t txBufSize)
{
  m_rbuf = reinterpret_cast<uint8_t*>(malloc(reassBufSize));
  m_rbufSize = reassBufSize;
  m_offset = 0;
  m_tbuf = reinterpret_cast<uint8_t*>(malloc(txBufSize));
  m_tbufSize = txBufSize;
}

void
LiteFrag::end()
{
  if (m_rbuf != nullptr) {
    free(m_rbuf);
    m_rbuf = nullptr;
  }
  if (m_tbuf != nullptr) {
    free(m_tbuf);
    m_tbuf = nullptr;
  }
}

size_t
LiteFrag::receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId)
{
  if (m_rbuf == nullptr) {
    LITEFRAG_DBG(F("receive err=no-buffer"));
    return 0;
  }

  while (true) {
    ReassHdr* hdr = reinterpret_cast<ReassHdr*>(m_rbuf + m_offset);
    size_t room = m_rbufSize - m_offset - sizeof(hdr->len);
    uint8_t* fragBuf = &hdr->b0;
    uint16_t fragLen = static_cast<uint16_t>(inner.receive(fragBuf, room, endpointId));
    if (fragLen == 0) { // no incoming packet
      return 0;
    }
    if ((hdr->b0 & LiteFragHdr_b0_HB) == 0) { // no header bit
      continue;
    }
    memcpy(&hdr->len, &fragLen, sizeof(hdr->len));
    uint16_t totalLen = sizeof(hdr->len) + fragLen;

    uint16_t id;
    memcpy(&id, &hdr->id, sizeof(id));
    uint8_t seq = hdr->b0 & LiteFragHdr_b0_SEQ_MASK;

    if (seq == 0) {
      if (m_offset != 0) {
        LITEFRAG_DBG(F("discard-incomplete id=") << m_id << F(" frags=") << m_nextSeq);
        memmove(m_rbuf, hdr, totalLen);
        hdr = reinterpret_cast<ReassHdr*>(m_rbuf);
        m_offset = 0;
      }
    }
    else if (id != m_id || seq != m_nextSeq) {
      LITEFRAG_DBG(F("drop-ooo want=(") << m_id << "," << m_nextSeq <<
                   F(") got=(") << id << "," << seq << ")");
      continue;
    }

    // accept
    m_offset += totalLen;
    m_id = id;
    m_nextSeq = seq + 1;

    if ((hdr->b0 & LiteFragHdr_b0_MF) != 0) { // last frag
      size_t size = reassemble(buf, bufSize);
      m_offset = 0;
      return size;
    }
  }
  return 0;
}

size_t
LiteFrag::reassemble(uint8_t* buf, size_t bufSize)
{
  int nFrags = 0;
  size_t size = 0;
  for (size_t offset = 0; offset < m_offset;) {
    ReassHdr* hdr = reinterpret_cast<ReassHdr*>(m_rbuf + offset);
    uint16_t fragLen;
    memcpy(&fragLen, &hdr->len, sizeof(fragLen));
    uint16_t payloadLen = fragLen - 3;
    uint16_t totalLen = sizeof(hdr->len) + fragLen;

    if (payloadLen > size - bufSize) {
      LITEFRAG_DBG(F("reassemble err=no-room"));
      return 0;
    }

    memcpy(buf + size, hdr->payload, payloadLen);
    size += payloadLen;
    offset += totalLen;
    ++nFrags;
  }

  LITEFRAG_DBG(F("reassemble id=") << m_id << F(" frags=") << nFrags);
  return size;
}

ndn_Error
LiteFrag::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  if (m_tbuf == nullptr) {
    LITEFRAG_DBG(F("send err=no-buffer"));
    return NDN_ERROR_SocketTransport_cannot_connect_to_socket;
  }

  uint16_t id = random(0xFFFF);
  int seq = 0;
  for (size_t offset = 0; offset < len;) {
    size_t payloadLen = std::min(len - offset, m_tbufSize - 3);
    m_tbuf[0] = LiteFragHdr_b0_HB | (seq++ & LiteFragHdr_b0_SEQ_MASK);
    memcpy(m_tbuf + 1, &id, sizeof(id));
    memcpy(m_tbuf + 3, pkt + offset, payloadLen);

    offset += payloadLen;
    if (offset == len) {
      m_tbuf[0] |= LiteFragHdr_b0_MF;
    }

    ndn_Error err = inner.send(m_tbuf, 3 + payloadLen, endpointId);
    if (err != NDN_ERROR_success) {
      LITEFRAG_DBG(F("send id=") << id << F(" seq=") << seq << F(" err=") << err);
      return err;
    }
    yield();
  }

  LITEFRAG_DBG(F("send id=") << id << F(" seqs=") << seq);
}

} // namespace ndn