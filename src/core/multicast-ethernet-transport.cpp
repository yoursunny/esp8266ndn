#include "multicast-ethernet-transport.hpp"
#include "logger.hpp"

#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>

#if defined(ESP8266)
#include <array>
#elif defined(ESP32)
#include <freertos/queue.h>
#endif

#define MCASTETHTRANSPORT_DBG(...) DBG(MulticastEthernetTransport, __VA_ARGS__)

namespace ndn {

#if defined(ESP8266)
class MulticastEthernetTransport::Queue
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
  std::array<pbuf*, MCASTETHTRANSPORT_RX_QUEUE_LEN> m_arr;
  int m_head;
  int m_tail;
};
#elif defined(ESP32)
class MulticastEthernetTransport::Queue
{
public:
  Queue()
  {
    m_queue = xQueueCreate(MCASTETHTRANSPORT_RX_QUEUE_LEN, sizeof(pbuf*));
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

static MulticastEthernetTransport* g_multicastEthernetTransport = nullptr;

class MulticastEthernetTransport::Impl
{
public:
  static err_t
  input(pbuf* p, netif* inp);
};

err_t
MulticastEthernetTransport::Impl::input(pbuf* p, netif* inp)
{
  if (g_multicastEthernetTransport == nullptr ||
      g_multicastEthernetTransport->m_netif != inp) {
    pbuf_free(p);
    return ERR_OK;
  }

  if (p->len >= sizeof(eth_hdr)) {
    const eth_hdr* eth = reinterpret_cast<const eth_hdr*>(p->payload);
    if (eth->type == PP_HTONS(0x8624)) {
      bool ok = g_multicastEthernetTransport->m_queue->push(p);
      if (!ok) {
        MCASTETHTRANSPORT_DBG(F("RX queue is full"));
        pbuf_free(p);
      }
      return ERR_OK;
    }
  }

  return reinterpret_cast<netif_input_fn>(g_multicastEthernetTransport->m_oldInput)(p, inp);
}

void
MulticastEthernetTransport::listNetifs(Print& os)
{
  for (netif* netif = netif_list; netif != nullptr; netif = netif->next) {
    os << netif->name[0] << netif->name[1] << netif->num << endl;
  }
}

MulticastEthernetTransport::MulticastEthernetTransport()
  : m_netif(nullptr)
  , m_oldInput(nullptr)
{
}

bool
MulticastEthernetTransport::begin(const char ifname[2], uint8_t ifnum)
{
  if (LWIP_VERSION_MAJOR != 1) {
    MCASTETHTRANSPORT_DBG(F("packet interception on lwip ") << LWIP_VERSION_MAJOR << "." <<
                          LWIP_VERSION_MINOR << F(" is not tested and may not work"));
  }
  if (m_netif != nullptr) {
    MCASTETHTRANSPORT_DBG(F("this instance is active"));
    return false;
  }
  if (g_multicastEthernetTransport != nullptr) {
    MCASTETHTRANSPORT_DBG(F("another instance is active"));
    return false;
  }

  for (netif* netif = netif_list; netif != nullptr; netif = netif->next) {
    if (netif->name[0] == ifname[0] && netif->name[1] == ifname[1] &&
        netif->num == ifnum) {
      m_netif = netif;
      break;
    }
  }
  if (m_netif == nullptr) {
    MCASTETHTRANSPORT_DBG(F("netif ") << ifname[0] << ifname[1] << ifnum << F(" not found"));
    return false;
  }

  m_queue = new Queue();
  g_multicastEthernetTransport = this;
  m_oldInput = reinterpret_cast<void*>(m_netif->input);
  m_netif->input = MulticastEthernetTransport::Impl::input;
  MCASTETHTRANSPORT_DBG(F("enabled on ") << ifname[0] << ifname[1] << ifnum);
}

void
MulticastEthernetTransport::end()
{
  g_multicastEthernetTransport = nullptr;
  pbuf* p;
  while ((p = m_queue->pop()) != nullptr) {
    pbuf_free(p);
  }
  delete m_queue;
  m_netif->input = reinterpret_cast<netif_input_fn>(m_oldInput);
  m_oldInput = nullptr;
  m_netif = nullptr;
  MCASTETHTRANSPORT_DBG(F("disabled"));
}

size_t
MulticastEthernetTransport::receive(uint8_t* buf, size_t bufSize, uint64_t* endpointId)
{
  size_t pktSize = 0;
  pbuf* p;
  while ((p = m_queue->pop()) != nullptr) {
    if (p->next != nullptr) {
      MCASTETHTRANSPORT_DBG(F("unhandled: chained packet"));
      pbuf_free(p);
      continue;
    }

    if (p->tot_len > bufSize) {
      MCASTETHTRANSPORT_DBG(F("insufficient receive buffer: tot_len=") << _DEC(p->tot_len));
      pbuf_free(p);
      continue;
    }

    const eth_hdr* eth = reinterpret_cast<const eth_hdr*>(p->payload);
    *endpointId = 0;
    memcpy(endpointId, &eth->src, 6);

    pktSize = p->tot_len - sizeof(eth_hdr);
    memcpy(buf, reinterpret_cast<const uint8_t*>(p->payload) + sizeof(eth_hdr), pktSize);

    pbuf_free(p);
    break;
  }
  return pktSize;
}

ndn_Error
MulticastEthernetTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  uint16_t frameSize = sizeof(eth_hdr) + len;
  pbuf* p = pbuf_alloc(PBUF_RAW, frameSize, PBUF_RAM);
  if (p == nullptr) {
    return NDN_ERROR_DynamicUInt8Array_realloc_failed;
  }
  p->len = p->tot_len = frameSize;

  eth_hdr* eth = reinterpret_cast<eth_hdr*>(p->payload);
  ETHADDR32_COPY(&eth->dest, "\x01\x00\x5E\x00\x17\xAA");
  ETHADDR16_COPY(&eth->src, m_netif->hwaddr);
  eth->type = PP_HTONS(0x8624);
  memcpy(reinterpret_cast<uint8_t*>(p->payload) + sizeof(eth_hdr), pkt, len);

  err_t e = m_netif->linkoutput(m_netif, p);
  pbuf_free(p);
  if (e != ERR_OK) {
    MCASTETHTRANSPORT_DBG(F("linkoutput error ") << _DEC(e));
    return NDN_ERROR_SocketTransport_error_in_send;
  }

  return NDN_ERROR_success;
}

} // namespace ndn