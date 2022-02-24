#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "ethernet-transport.hpp"
#include "../core/logger.hpp"

#include <IPAddress.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>

#define LOG(...) LOGGER(EthernetTransport, __VA_ARGS__)
#define NDN_ETHERTYPE_BE (PP_HTONS(0x8624))

namespace esp8266ndn {

namespace {

union EndpointId
{
  uint64_t id;
  struct
  {
    uint8_t addr[6];
    bool isMulticast;
  };
};

static_assert(sizeof(EndpointId) == sizeof(uint64_t), "");

} // anonymous namespace

static EthernetTransport* g_ethTransport = nullptr;

class EthernetTransport::Impl
{
public:
  explicit Impl(netif* nif)
    : nif(nif)
    , oldInput(nif->input)
  {
#if defined(ARDUINO_ARCH_ESP32)
    nif->input = EthernetTransport::Impl::input;
#elif defined(HAS_PHY_CAPTURE) && HAS_PHY_CAPTURE
    ::phy_capture = capture;
#endif
  }

  ~Impl()
  {
#if defined(ARDUINO_ARCH_ESP32)
    nif->input = this->oldInput;
#elif defined(HAS_PHY_CAPTURE) && HAS_PHY_CAPTURE
    ::phy_capture = nullptr;
#endif
  }

#if defined(ARDUINO_ARCH_ESP32)
  static err_t input(pbuf* p, netif* inp)
  {
    if (g_ethTransport == nullptr) {
      LOG(F("inactive"));
      pbuf_free(p);
      return ERR_OK;
    }
    Impl& self = *g_ethTransport->m_impl;

    const eth_hdr* eth = reinterpret_cast<const eth_hdr*>(p->payload);
    if (self.nif != inp || p->len < sizeof(eth_hdr) || eth->type != NDN_ETHERTYPE_BE) {
      return self.oldInput(p, inp);
    }

    if (p->next != nullptr) {
      LOG(F("drop: chained packet"));
    } else {
      self.receive(reinterpret_cast<const uint8_t*>(p->payload), p->tot_len);
    }
    pbuf_free(p);
    return ERR_OK;
  }
#elif defined(HAS_PHY_CAPTURE) && HAS_PHY_CAPTURE
  static void capture(int ifindex, const char* payload, size_t size, int out, int success)
  {
    if (out != 0 || success != 1) {
      return;
    }

    if (g_ethTransport == nullptr) {
      LOG(F("inactive"));
      return;
    }
    Impl& self = *g_ethTransport->m_impl;
    if (ifindex != static_cast<int>(self.nif->num)) {
      return;
    }

    self.receive(reinterpret_cast<const uint8_t*>(payload), size);
  }
#endif

  void receive(const uint8_t* payload, size_t size)
  {
    const eth_hdr* eth = reinterpret_cast<const eth_hdr*>(payload);
    if (size < sizeof(eth_hdr) || eth->type != NDN_ETHERTYPE_BE) {
      return;
    }

    auto r = g_ethTransport->receiving();
    if (!r) {
      LOG(F("drop: no RX buffer"));
      return;
    }

    if (size > r.bufLen()) {
      LOG(F("drop: RX buffer too short, size=") << _DEC(size));
      return;
    }

    EndpointId endpoint{};
    memcpy(endpoint.addr, &eth->src, 6);
    endpoint.isMulticast = 0x01 & (*reinterpret_cast<const uint8_t*>(&eth->dest));

    size_t pktLen = size - sizeof(eth_hdr);
    memcpy(r.buf(), payload + sizeof(eth_hdr), pktLen);
    r(pktLen, endpoint.id);
  }

public:
  netif* nif = nullptr;
  netif_input_fn oldInput = nullptr;
};

void
EthernetTransport::listNetifs(Print& os)
{
  for (netif* nif = netif_list; nif != nullptr; nif = nif->next) {
    uint32_t ip = 0;
    std::copy_n(reinterpret_cast<uint8_t*>(&nif->ip_addr), sizeof(ip),
                reinterpret_cast<uint8_t*>(&ip));
    os << nif->name[0] << nif->name[1] << nif->num << ' ' << IPAddress(ip) << endl;
  }
}

EthernetTransport::EthernetTransport() = default;

EthernetTransport::~EthernetTransport()
{
  end();
}

bool
EthernetTransport::begin(const char ifname[2], uint8_t ifnum)
{
  netif* found = nullptr;
  for (netif* nif = netif_list; nif != nullptr; nif = nif->next) {
    if (nif->name[0] == ifname[0] && nif->name[1] == ifname[1] && nif->num == ifnum) {
      found = nif;
      break;
    }
  }
  if (found == nullptr) {
    LOG(F("netif ") << ifname[0] << ifname[1] << ifnum << F(" not found"));
    return false;
  }
  return begin(found);
}

bool
EthernetTransport::begin()
{
  for (netif* nif = netif_list; nif != nullptr; nif = nif->next) {
    if (nif->name[0] == 's' && nif->name[1] == 't') {
      return begin(nif);
    }
  }
  LOG(F("no available netif"));
  return false;
}

bool
EthernetTransport::begin(netif* nif)
{
  if (g_ethTransport != nullptr) {
    LOG(F("an instance is active"));
    return false;
  }

  g_ethTransport = this;
  m_impl.reset(new Impl(nif));
  LOG(F("enabled on ") << nif->name[0] << nif->name[1] << nif->num);
  return true;
}

void
EthernetTransport::end()
{
  if (m_impl == nullptr) {
    return;
  }
  m_impl.reset();
  g_ethTransport = nullptr;
  LOG(F("disabled"));
}

bool
EthernetTransport::doIsUp() const
{
  return m_impl != nullptr;
}

void
EthernetTransport::doLoop()
{
  loopRxQueue();
}

bool
EthernetTransport::doSend(const uint8_t* pkt, size_t pktLen, uint64_t endpointId)
{
  if (m_impl == nullptr) {
    return false;
  }

  uint16_t payloadLen = std::max<uint16_t>(pktLen, 46);
  uint16_t frameSize = sizeof(eth_hdr) + payloadLen;
  pbuf* p = pbuf_alloc(PBUF_RAW_TX, frameSize, PBUF_RAM);
  if (p == nullptr) {
    return false;
  }
  p->len = p->tot_len = frameSize;

  eth_hdr* eth = reinterpret_cast<eth_hdr*>(p->payload);
  EndpointId endpoint;
  endpoint.id = endpointId;
  if (endpointId == 0 || endpoint.isMulticast) {
    memcpy(&eth->dest, "\x01\x00\x5E\x00\x17\xAA", sizeof(eth->dest));
  } else {
    memcpy(&eth->dest, endpoint.addr, sizeof(eth->dest));
  }
  memcpy(&eth->src, m_impl->nif->hwaddr, sizeof(eth->src));
  eth->type = NDN_ETHERTYPE_BE;
  memcpy(reinterpret_cast<uint8_t*>(p->payload) + sizeof(eth_hdr), pkt, pktLen);
  if (pktLen < payloadLen) {
    memset(reinterpret_cast<uint8_t*>(p->payload) + sizeof(eth_hdr) + pktLen, 0,
           payloadLen - pktLen);
  }

  err_t e = m_impl->nif->linkoutput(m_impl->nif, p);
  pbuf_free(p);
  if (e != ERR_OK) {
    LOG(F("linkoutput error ") << _DEC(e));
    return false;
  }
  return true;
}

} // namespace esp8266ndn

#endif // defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
