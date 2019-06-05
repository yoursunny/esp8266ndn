#ifndef ESP8266NDN_TRANSPORT_HPP
#define ESP8266NDN_TRANSPORT_HPP

#include "../core/packet-buffer.hpp"
#include "detail/queue.hpp"

namespace ndn {

/** \brief a Transport sends and receives NDN packets over the network
 *
 *  Transport exposes a push-mode RX side API. Face receives incoming packets
 *  via ReceiveCallback, which should be invoked on the same thread as loop().
 */
class Transport
{
public:
  virtual
  ~Transport();

  virtual void
  loop();

  typedef void (*ReceiveCallback)(void* arg, PacketBuffer* pb);

  /** \brief set a callback to be invoked when a packet is received
   */
  void
  onReceive(ReceiveCallback cb, void* cbarg);

  /** \brief determine whether there's room for more receive buffers
   */
  bool
  canPushReceiveBuffer() const;

  /** \brief add a receive buffer
   */
  bool
  pushReceiveBuffer(PacketBuffer* pb);

  /** \brief send a packet
   *  \param pkt packet to send
   *  \param len packet size
   *  \param endpointId identifier of the remote endpoint
   */
  virtual ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) = 0;

protected:
  /** \brief obtain an empty receive buffer
   */
  PacketBuffer*
  beforeReceive();

  /** \brief notify a packet has been received, or return an empty buffer
   *  \param pktSize if zero, pb is returned as empty buffer
   *  \param isAsync if true, ReceiveCallback cannot be invoked within this function
   */
  void
  afterReceive(PacketBuffer* pb, size_t pktSize, bool isAsync);

  void
  invokeRxCb();

private:
  ReceiveCallback m_rxCb = nullptr;
  void* m_rxCbArg = nullptr;

  static constexpr int s_rxQueueCap = 4;
  detail::SafeQueue<PacketBuffer*, s_rxQueueCap> m_rxQueueIn;
  detail::SafeQueue<PacketBuffer*, s_rxQueueCap> m_rxQueueOut;
};

/** \brief a PollModeTransport converts poll-mode RX to push-mode RX
 */
class PollModeTransport : public Transport
{
public:
  void
  loop() override;

  /** \brief receive a packet
   *  \param buf receive buffer
   *  \param bufSize receive buffer size
   *  \param[out] endpointId identifier of the remote endpoint
   *  \return size of received packet, or zero if no packet available
   */
  virtual size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId) = 0;
};

} // namespace ndn

#endif // ESP8266NDN_TRANSPORT_HPP
