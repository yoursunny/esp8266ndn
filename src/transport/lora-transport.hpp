#ifndef ESP8266NDN_LORA_TRANSPORT_HPP
#define ESP8266NDN_LORA_TRANSPORT_HPP

#include "transport.hpp"

namespace ndn {

/** \brief a transport that communicates over LoRa radio
 *  \tparam Lora LoRaClass from Arduino LoRa library
 *
 *  This is written as a template to avoid hard dependency on LoRa.h.
 *  It should be initialized like this:
 *  \code
 *  #include <LoRa.h>
 *  ndn::LoraTransport<LoRaClass> transport(LoRa);
 *  void setup() {
 *    LoRa.begin(..);
 *    // there is no transport.begin()
 *  }
 *  \endcode
 */
template<typename Lora>
class LoraTransport : public Transport
{
public:
  explicit
  LoraTransport(Lora& lora)
    : m_lora(lora)
  {
  }

  size_t
  receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId) override
  {
    if (m_lora.parsePacket() <= 0) {
      return 0;
    }
    return m_lora.readBytes(buf, bufSize);
  }

  ndn_Error
  send(const uint8_t* pkt, size_t len, uint64_t endpointId) override
  {
    if (!m_lora.beginPacket()) {
      return NDN_ERROR_SocketTransport_cannot_connect_to_socket;
    }
    m_lora.write(pkt, len);
    return m_lora.endPacket() ? NDN_ERROR_success : NDN_ERROR_SocketTransport_error_in_send;
  }

private:
  Lora& m_lora;
};

} // namespace ndn

#endif // ESP8266NDN_LORA_TRANSPORT_HPP
