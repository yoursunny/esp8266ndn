#include "nlface.hpp"
#include "detail/nlintf.hpp"
#include "../core/logger.hpp"

#include "../ndn-lite/ndn-error-code.h"

#define LOG(...) LOGGER(NlFace, __VA_ARGS__)

namespace ndn {

class NlFace::Impl : public detail::NlIntf
{
public:
  explicit
  Impl(Transport& transport)
    : NlIntf(NDN_FACE_TYPE_NET, send)
    , m_transport(transport)
  {
    // TODO consolidate pushReceiveBuffer with Face class
    m_transport.pushReceiveBuffer(new PacketBuffer({}));

    m_transport.onReceive(Impl::receive, this);
  }

private:
  static int
  send(ndn_face_intf_t* self, const uint8_t* pkt, uint32_t len)
  {
    Impl* impl = static_cast<Impl*>(self);
    LOG(_DEC(impl->face_id) << F(" TX len=") << _DEC(len));
    LOG(_DEC(impl->face_id) << " TX " << PrintHex(pkt, len));
    ndn_Error e = impl->m_transport.send(pkt, len, 0);
    return e == NDN_ERROR_success ? NDN_SUCCESS : NDN_OVERSIZE;
  }

  static void
  receive(void* arg, PacketBuffer* pb)
  {
    const uint8_t* pkt = nullptr;
    size_t len = 0;
    std::tie(pkt, len) = pb->getRaw(PacketBuffer::RAW_L3);

    Impl* impl = static_cast<Impl*>(arg);
    LOG(_DEC(impl->face_id) << F(" RX len=") << _DEC(len));
    LOG(_DEC(impl->face_id) << " RX " << PrintHex(pkt, len));
    int res = impl->intfReceive(pkt, len);
    LOG(_HEX(impl->face_id) << F(" RX res=") << _DEC(res));
    impl->m_transport.pushReceiveBuffer(pb);
  }

public:
  Transport& m_transport;
};

NlFace::NlFace(Transport& transport)
  : m_impl(new Impl(transport))
{
}

NlFace::~NlFace() = default;

bool
NlFace::begin()
{
  return m_impl->begin();
}

bool
NlFace::end()
{
  return m_impl->end();
}

ndn_Error
NlFace::addRoute(const NameLite& name)
{
  return m_impl->addRoute(name);
}

} // namespace ndn
