#ifndef ESP8266NDN_LITE_FRAG_HPP
#define ESP8266NDN_LITE_FRAG_HPP

#include "transport.hpp"

namespace ndn {

/** \brief a transport wrapper that supports NDN-Lite Fragmentation protocol
 *  \sa https://github.com/named-data-iot/ndn-lite-wiki/blob/0b9b7fa7c2fef81f24030b69d7b256c36fb30f91/Overview.md#features
 */
class LiteFrag : public Transport
{
public:
  explicit
  LiteFrag(Transport& inner);

  ~LiteFrag();

  /** \begin allocate buffers
   *  \param reassBufSize reassembly buffer size to store fragments of a packet
   *  \param txBufSize transmission buffer size to store one outgoing fragment
   */
  void
  begin(size_t reassBufSize, size_t txBufSize);

  /** \begin release buffers
   */
  void
  end();

  /** \begin receive a reassembled packet
   *
   *  Current implementation only accommodates in-order delivery;
   *  out-of-order fragments will be dropped.
   */
  size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId) final;

  /** \begin transmit fragments of a packet
   */
  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) final;

private:
  size_t
  reassemble(uint8_t* buf, size_t bufSize);

public:
  Transport& inner;

private:
  uint8_t* m_rbuf;
  size_t m_rbufSize;
  size_t m_offset;
  uint16_t m_id;
  uint8_t m_nextSeq;

  uint8_t* m_tbuf;
  size_t m_tbufSize;
};

} // namespace ndn

#endif // ESP8266NDN_LITE_FRAG_HPP
