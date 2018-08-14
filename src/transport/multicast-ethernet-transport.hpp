#ifndef ESP8266NDN_MULTICAST_ETHERNET_TRANSPORT_HPP
#define ESP8266NDN_MULTICAST_ETHERNET_TRANSPORT_HPP

#include "transport.hpp"

extern "C" {
struct netif;
struct pbuf;
}
class Print;

namespace ndn {

/** \brief Receive queue length of MulticastEthernetTransport.
 */
#define MCASTETHTRANSPORT_RX_QUEUE_LEN 4

/** \brief a transport that communicates over Ethernet multicast
 */
class MulticastEthernetTransport : public Transport
{
public:
  static void
  listNetifs(Print& os);

  MulticastEthernetTransport();

  /** \brief Start intercepting NDN packets on a network interface.
   *  \return whether success
   */
  bool
  begin(const char ifname[2], uint8_t ifnum);

  /** \brief Start intercepting NDN packets on any station interface.
   *  \return whether success
   */
  bool
  begin();

  void
  end();

  size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t* endpointId) final;

  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) final;

private:
  class Impl;
  class Queue;

  netif* m_netif;
  void* m_oldInput;

  /** \brief The receive queue.
   *
   *  This transport places intercepted packets in RX queue to be receive()'ed
   *  later, instead of posting them via a callback, for two reasons:
   *  (1) netif_input_fn must not block network stack for too long.
   *  (2) ESP32 executes netif_input_fn in CPU0 and the Arduino main loop in
   *      CPU1. Using the RX queue keeps the application in CPU1, and does not
   *      have to deal with multi-threading.
   */
  Queue* m_queue;
};

} // namespace ndn

#endif // ESP8266NDN_MULTICAST_ETHERNET_TRANSPORT_HPP
