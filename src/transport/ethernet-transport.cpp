#if defined(ESP8266) || defined(ESP32)

#include "ethernet-transport.hpp"
#include "detail/queue.hpp"
#include "../core/logger.hpp"

#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>
#include <IPAddress.h>

#define ETHTRANSPORT_DBG(...) DBG(EthernetTransport, __VA_ARGS__)

namespace ndn {

static EthernetTransport* g_ethTransport = nullptr;

class EthernetTransport::Impl
{
public:
  explicit
  Impl(netif* nif)
    : nif(nif)
    , oldInput(nif->input)
  {
    nif->input = EthernetTransport::Impl::input;
  }

  ~Impl()
  {
    nif->input = this->oldInput;
    bool ok;
    pbuf* p;
    while (std::tie(p, ok) = queue.pop(), ok) {
      pbuf_free(p);
    }
  }

  static err_t
  input(pbuf* p, netif* inp)
  {
    if (g_ethTransport == nullptr) {
      ETHTRANSPORT_DBG(F("inactive"));
      pbuf_free(p);
      return ERR_OK;
    }

    Impl& self = *g_ethTransport->m_impl;
    const eth_hdr* eth = reinterpret_cast<const eth_hdr*>(p->payload);
    if (self.nif != inp || p->len < sizeof(eth_hdr) || eth->type != PP_HTONS(0x8624)) {
      return self.oldInput(p, inp);
    }

    bool ok = self.queue.push(p);
    if (!ok) {
      ETHTRANSPORT_DBG(F("RX queue is full"));
      pbuf_free(p);
    }
    return ERR_OK;
  }

public:
  netif* nif = nullptr;
  netif_input_fn oldInput = nullptr;

  /** \brief The receive queue.
   *
   *  This transport places intercepted packets in RX queue to be receive()'ed
   *  later, instead of posting them via a callback, for two reasons:
   *  (1) netif_input_fn must not block network stack for too long.
   *  (2) ESP32 executes netif_input_fn in CPU0 and the Arduino main loop in
   *      CPU1. Using the RX queue keeps the application in CPU1, so it does
   *      not have to deal with multi-threading.
   */
  detail::SafeQueue<pbuf*, ETHTRANSPORT_RX_QUEUE_LEN> queue;
};

void
EthernetTransport::listNetifs(Print& os)
{
  for (netif* nif = netif_list; nif != nullptr; nif = nif->next) {
    os << nif->name[0] << nif->name[1] << nif->num
       << ' ' << IPAddress(*reinterpret_cast<const uint32_t*>(&nif->ip_addr)) << endl;
  }
}

EthernetTransport::EthernetTransport() = default;

EthernetTransport::~EthernetTransport() = default;

bool
EthernetTransport::begin(const char ifname[2], uint8_t ifnum)
{
  netif* found = nullptr;
  for (netif* nif = netif_list; nif != nullptr; nif = nif->next) {
    if (nif->name[0] == ifname[0] && nif->name[1] == ifname[1] &&
        nif->num == ifnum) {
      found = nif;
      break;
    }
  }
  if (found == nullptr) {
    ETHTRANSPORT_DBG(F("netif ") << ifname[0] << ifname[1] << ifnum << F(" not found"));
    return false;
  }
  return begin(found);
}

bool
EthernetTransport::begin()
{
#if defined(ESP8266) && LWIP_VERSION_MAJOR == 1
  return begin("ew", 0);
#else
  for (netif* nif = netif_list; nif != nullptr; nif = nif->next) {
    if (nif->name[0] == 's' && nif->name[1] == 't') {
      return begin(nif);
    }
  }
  ETHTRANSPORT_DBG(F("no available netif"));
  return false;
#endif
}

bool
EthernetTransport::begin(netif* nif)
{
#ifdef ESP8266
  if (LWIP_VERSION_MAJOR != 1) {
    ETHTRANSPORT_DBG(F("packet interception on ESP8266 lwip2 is untested and may not work"));
  }
#endif // ESP8266
  if (g_ethTransport != nullptr) {
    ETHTRANSPORT_DBG(F("an instance is active"));
    return false;
  }

  g_ethTransport = this;
  m_impl.reset(new Impl(nif));
  ETHTRANSPORT_DBG(F("enabled on ") << nif->name[0] << nif->name[1] << nif->num);
  return true;
}

void
EthernetTransport::end()
{
  m_impl.reset();
  g_ethTransport = nullptr;
  ETHTRANSPORT_DBG(F("disabled"));
}

size_t
EthernetTransport::receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId)
{
  if (m_impl == nullptr) {
    return 0;
  }

  size_t pktSize = 0;
  pbuf* p;
  bool ok;
  while (std::tie(p, ok) = m_impl->queue.pop(), ok) {
    if (p->next != nullptr) {
      ETHTRANSPORT_DBG(F("unhandled: chained packet"));
      pbuf_free(p);
      continue;
    }

    if (p->tot_len > bufSize) {
      ETHTRANSPORT_DBG(F("insufficient receive buffer: tot_len=") << _DEC(p->tot_len));
      pbuf_free(p);
      continue;
    }

    const eth_hdr* eth = reinterpret_cast<const eth_hdr*>(p->payload);
    EndpointId endpoint = {0};
    memcpy(endpoint.addr, &eth->src, 6);
    endpoint.isMulticast = 0x01 & (*reinterpret_cast<const uint8_t*>(&eth->dest));
    endpointId = endpoint.endpointId;

    pktSize = p->tot_len - sizeof(eth_hdr);
    memcpy(buf, reinterpret_cast<const uint8_t*>(p->payload) + sizeof(eth_hdr), pktSize);

    pbuf_free(p);
    break;
  }
  return pktSize;
}

ndn_Error
EthernetTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  if (m_impl == nullptr) {
    return NDN_ERROR_SocketTransport_socket_is_not_open;
  }

  uint16_t payloadLen = max(static_cast<uint16_t>(len), static_cast<uint16_t>(46));
  uint16_t frameSize = sizeof(eth_hdr) + payloadLen;
  pbuf* p = pbuf_alloc(PBUF_RAW, frameSize, PBUF_RAM);
  if (p == nullptr) {
    return NDN_ERROR_DynamicUInt8Array_realloc_failed;
  }
  p->len = p->tot_len = frameSize;

  eth_hdr* eth = reinterpret_cast<eth_hdr*>(p->payload);
  if (endpointId == 0) {
    memcpy(&eth->dest, "\x01\x00\x5E\x00\x17\xAA", sizeof(eth->dest));
  }
  else {
    EndpointId endpoint;
    endpoint.endpointId = endpointId;
    memcpy(&eth->dest, endpoint.addr, sizeof(eth->dest));
  }
  memcpy(&eth->src, m_impl->nif->hwaddr, sizeof(eth->src));
  eth->type = PP_HTONS(0x8624);
  memcpy(reinterpret_cast<uint8_t*>(p->payload) + sizeof(eth_hdr), pkt, len);
  if (len < payloadLen) {
    memset(reinterpret_cast<uint8_t*>(p->payload) + sizeof(eth_hdr) + len, 0, payloadLen - len);
  }

  err_t e = m_impl->nif->linkoutput(m_impl->nif, p);
  pbuf_free(p);
  if (e != ERR_OK) {
    ETHTRANSPORT_DBG(F("linkoutput error ") << _DEC(e));
    return NDN_ERROR_SocketTransport_error_in_send;
  }

  return NDN_ERROR_success;
}

} // namespace ndn

#endif // defined(ESP8266) || defined(ESP32)
