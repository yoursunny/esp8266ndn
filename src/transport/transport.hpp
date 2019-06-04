#ifndef ESP8266NDN_TRANSPORT_HPP
#define ESP8266NDN_TRANSPORT_HPP

#include "../core/packet-buffer.hpp"

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

  typedef void (*ReceiveCallback)(void* arg, PacketBuffer* pb, uint64_t endpointId);

  /** \brief set a callback to be invoked when a packet is received
   */
  void
  onReceive(ReceiveCallback cb, void* cbarg);

  /** \brief return number of receive buffers
   */
  size_t
  countReceiveBuffer() const;

  /** \brief add a receive buffer
   */
  void
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

  /** \brief notify a packet has been received
   */
  void
  afterReceive(PacketBuffer* pb, size_t pktSize, uint64_t endpointId);

private:
  ReceiveCallback m_rxCb = nullptr;
  void* m_rxCbArg = nullptr;
  PacketBuffer* m_pb = nullptr;
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
