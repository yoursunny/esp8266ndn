#ifndef ESP8266NDN_TRANSPORT_ETHERNET_TRANSPORT_HPP
#define ESP8266NDN_TRANSPORT_ETHERNET_TRANSPORT_HPP

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "../port/port.hpp"

extern "C"
{
  struct netif;
}
class Print;

namespace esp8266ndn {

/** @brief A transport that communicates over Ethernet. */
class EthernetTransport
  : public virtual ndnph::Transport
  , public ndnph::transport::DynamicRxQueueMixin
{
public:
  /** @brief Print a list of network interfaces. */
  static void listNetifs(Print& os);

  EthernetTransport();

  ~EthernetTransport() override;

  /**
   * @brief Start intercepting NDN packets on a network interface.
   * @return whether success.
   */
  bool begin(const char ifname[2], uint8_t ifnum);

  /**
   * @brief Start intercepting NDN packets on first available station interface.
   * @return whether success.
   */
  bool begin();

  /** @brief Disable the transport. */
  void end();

private:
  bool begin(netif* netif);

  bool doIsUp() const final;

  void doLoop() final;

  bool doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId) final;

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#endif // ESP8266NDN_TRANSPORT_ETHERNET_TRANSPORT_HPP
