#include "nltransport.hpp"
#include "detail/nlintf.hpp"
#include "../core/logger.hpp"

#include "../ndn-lite/ndn-error-code.h"

#define LOG(...) LOGGER(NlTransport, __VA_ARGS__)

namespace ndn {

class NlTransport::Impl : public detail::NlIntf
{
public:
  explicit
  Impl(NlTransport& transport)
    : NlIntf(NDN_FACE_TYPE_APP, receive)
    , m_transport(transport)
  {
  }

private:
  static int
  receive(ndn_face_intf_t* self, const uint8_t* pkt, uint32_t len)
  {
    Impl* impl = static_cast<Impl*>(self);
    NlTransport& transport = impl->m_transport;
    PacketBuffer* pb = transport.beforeReceive();
    if (pb == nullptr) {
      return NDN_OVERSIZE;
    }

    uint8_t* buf = nullptr;
    size_t bufSize = 0;
    std::tie(buf, bufSize) = pb->useBuffer();
    if (len > bufSize) {
      LOG(_HEX(impl->face_id) << F(" recv pb-too-short len=") << _DEC(len));
      transport.afterReceive(pb, 0, true);
      return NDN_OVERSIZE;
    }
    memcpy(buf, pkt, len);
    pb->endpointId = 0;

    ndn_Error e = transport.afterReceive(pb, len, true);
    return e == NDN_ERROR_success ? NDN_SUCCESS : NDN_OVERSIZE;
  }

public:
  NlTransport& m_transport;
};

NlTransport::NlTransport()
  : m_impl(new Impl(*this))
{
}

NlTransport::~NlTransport() = default;

bool
NlTransport::begin()
{
  return m_impl->begin();
}

bool
NlTransport::end()
{
  return m_impl->end();
}

ndn_Error
NlTransport::addRoute(const NameLite& name)
{
  return m_impl->addRoute(name);
}

ndn_Error
NlTransport::send(const uint8_t* pkt, size_t len, uint64_t endpointId)
{
  int res = m_impl->intfReceive(pkt, len);
  if (res != NDN_SUCCESS) {
    LOG(_HEX(m_impl->face_id) << F(" send res=") << _DEC(res) << "\n" << PrintHex(pkt, len));
  }

  switch (res) {
    case NDN_SUCCESS:
    case NDN_FWD_PIT_FULL:
    case NDN_FWD_INTEREST_REJECTED:
    case NDN_FWD_NO_ROUTE:
      return NDN_ERROR_success;
    default:
      return NDN_ERROR_SocketTransport_error_in_send;
  }
}

} // namespace ndn
