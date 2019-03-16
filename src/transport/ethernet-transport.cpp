#if defined(ESP8266) || defined(ESP32)

#include "ethernet-transport.hpp"
#include "../core/logger.hpp"

#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>
#include <IPAddress.h>

#if defined(ESP8266)
#include <array>
#elif defined(ESP32)
#include <freertos/queue.h>
#endif

#define ETHTRANSPORT_DBG(...) DBG(EthernetTransport, __VA_ARGS__)

namespace ndn {

#if defined(ESP8266)
class EthernetTransport::Queue
{
public:
  Queue()
    : m_head(0)
    , m_tail(0)
  {
  }

  bool
  push(pbuf* p)
  {
    int newTail = (m_tail + 1) % m_arr.size();
    if (newTail == m_head) {
      return false;
    }

    m_arr[m_tail] = p;
    m_tail = newTail;
    return true;
  }

  pbuf*
  pop()
  {
    if (m_head == m_tail) {
      return nullptr;
    }
    pbuf* p = m_arr[m_head];
    m_arr[m_head] = nullptr;
    m_head = (m_head + 1) % m_arr.size();
    return p;
  }

private:
  std::array<pbuf*, ETHTRANSPORT_RX_QUEUE_LEN> m_arr;
  int m_head;
  int m_tail;
};
#elif defined(ESP32)
class EthernetTransport::Queue
{
public:
  Queue()
  {
    m_queue = xQueueCreate(ETHTRANSPORT_RX_QUEUE_LEN, sizeof(pbuf*));
  }

  ~Queue()
  {
    vQueueDelete(m_queue);
  }

  bool
  push(pbuf* p)
  {
    BaseType_t res = xQueueSend(m_queue, &p, 0);
    return res == pdTRUE;
  }

  pbuf*
  pop()
  {
    pbuf* p;
    if (!xQueueReceive(m_queue, &p, 0)) {
      p = nullptr;
    }
    return p;
  }

private:
  QueueHandle_t m_queue;
};
#endif

static EthernetTransport* g_ethTransport = nullptr;

class EthernetTransport::Impl
{
public:
  static err_t
  input(pbuf* p, netif* inp);
};

err_t
EthernetTransport::Impl::input(pbuf* p, netif* inp)
{
  const eth_hdr* eth = reinterpret_cast<const eth_hdr*>(p->payload);
  if (g_ethTransport != nullptr && g_ethTransport->m_netif == inp &&
      p->len >= sizeof(eth_hdr) && eth->type == PP_HTONS(0x8624)) {
    bool ok = g_ethTransport->m_queue->push(p);
    if (!ok) {
      ETHTRANSPORT_DBG(F("RX queue is full"));
      pbuf_free(p);
    }
    return ERR_OK;
  }

  return reinterpret_cast<netif_input_fn>(g_ethTransport->m_oldInput)(p, inp);
}

void
EthernetTransport::listNetifs(Print& os)
{
  for (netif* netif = netif_list; netif != nullptr; netif = netif->next) {
    os << netif->name[0] << netif->name[1] << netif->num
       << ' ' << IPAddress(*reinterpret_cast<const uint32_t*>(&netif->ip_addr)) << endl;
  }
}

EthernetTransport::EthernetTransport()
  : m_netif(nullptr)
  , m_oldInput(nullptr)
{
}

bool
EthernetTransport::begin(const char ifname[2], uint8_t ifnum)
{
  netif* found = nullptr;
  for (netif* netif = netif_list; netif != nullptr; netif = netif->next) {
    if (netif->name[0] == ifname[0] && netif->name[1] == ifname[1] &&
        netif->num == ifnum) {
      found = netif;
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
  for (netif* netif = netif_list; netif != nullptr; netif = netif->next) {
    if (netif->name[0] == 's' && netif->name[1] == 't') {
      return begin(netif);
    }
  }
  ETHTRANSPORT_DBG(F("no available netif"));
  return false;
#endif
}

bool
EthernetTransport::begin(netif* netif)
{
#ifdef ESP8266
  if (LWIP_VERSION_MAJOR != 1) {
    ETHTRANSPORT_DBG(F("packet interception on ESP8266 lwip2 is untested and may not work"));
  }
#endif // ESP8266
  if (m_netif != nullptr) {
    ETHTRANSPORT_DBG(F("this instance is active"));
    return false;
  }
  if (g_ethTransport != nullptr) {
    ETHTRANSPORT_DBG(F("another instance is active"));
    return false;
  }

  m_netif = netif;
  m_queue = new Queue();
  g_ethTransport = this;
  m_oldInput = reinterpret_cast<void*>(m_netif->input);
  m_netif->input = EthernetTransport::Impl::input;
  ETHTRANSPORT_DBG(F("enabled on ") << netif->name[0] << netif->name[1] << netif->num);
  return true;
}

void
EthernetTransport::end()
{
  m_netif->input = reinterpret_cast<netif_input_fn>(m_oldInput);
  g_ethTransport = nullptr;
  pbuf* p;
  while ((p = m_queue->pop()) != nullptr) {
    pbuf_free(p);
  }
  delete m_queue;
  m_oldInput = nullptr;
  m_netif = nullptr;
  ETHTRANSPORT_DBG(F("disabled"));
}

size_t
EthernetTransport::receive(uint8_t* buf, size_t bufSize, uint64_t& endpointId)
{
  size_t pktSize = 0;
  pbuf* p;
  while ((p = m_queue->pop()) != nullptr) {
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
  uint16_t payloadLen = max(static_cast<uint16_t>(len), static_cast<uint16_t>(46));
  uint16_t frameSize = sizeof(eth_hdr) + payloadLen;
  pbuf* p = pbuf_alloc(PBUF_RAW, frameSize, PBUF_RAM);
  if (p == nullptr) {
    return NDN_ERROR_DynamicUInt8Array_realloc_failed;
  }
  p->len = p->tot_len = frameSize;
  if (frameSize > payloadLen) {
    memset(p->payload, 0, frameSize);
  }

  eth_hdr* eth = reinterpret_cast<eth_hdr*>(p->payload);
  if (endpointId == 0) {
    memcpy(&eth->dest, "\x01\x00\x5E\x00\x17\xAA", sizeof(eth->dest));
  }
  else {
    EndpointId endpoint;
    endpoint.endpointId = endpointId;
    memcpy(&eth->dest, endpoint.addr, sizeof(eth->dest));
  }
  memcpy(&eth->src, m_netif->hwaddr, sizeof(eth->src));
  eth->type = PP_HTONS(0x8624);
  memcpy(reinterpret_cast<uint8_t*>(p->payload) + sizeof(eth_hdr), pkt, len);

  err_t e = m_netif->linkoutput(m_netif, p);
  pbuf_free(p);
  if (e != ERR_OK) {
    ETHTRANSPORT_DBG(F("linkoutput error ") << _DEC(e));
    return NDN_ERROR_SocketTransport_error_in_send;
  }

  return NDN_ERROR_success;
}

} // namespace ndn

#endif // ESP8266NDN_UDP_TRANSPORT_HPP
