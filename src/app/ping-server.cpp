#include "ping-server.hpp"
#include "../core/logger.hpp"
#include "../core/uri.hpp"
#include "../core/with-components-buffer.hpp"

#define PINGSERVER_DBG(...) DBG(PingServer, __VA_ARGS__)

namespace ndn {

PingServer::PingServer(Face& face, const NameLite& prefix)
  : PacketHandler(face)
  , m_prefix(prefix)
  , m_probeCb(nullptr)
  , m_wantEndpointIdZero(false)
{
}

bool
PingServer::processInterest(const InterestLite& interest, uint64_t endpointId)
{
  if (!m_prefix.match(interest.getName())) {
    return false;
  }

  DataWCB<NDNPINGSERVER_NAMECOMPS_MAX, 0> data;
  data.getName().set(interest.getName());
  data.getMetaInfo().setFreshnessPeriod(0.0);
  PINGSERVER_DBG(F("processing request ") << PrintUri(interest.getName()));

  uint8_t payload[NDNPINGSERVER_PAYLOAD_MAX];
  size_t payloadSize = 0;
  if (m_probeCb != nullptr) {
    m_probeCb(m_probeCbArg, interest, payload, &payloadSize);
  }
  if (payloadSize > NDNPINGSERVER_PAYLOAD_MAX) {
    PINGSERVER_DBG(F("payload too large ") << payloadSize);
    return false;
  }

  data.setContent(BlobLite(payload, payloadSize));
  getFace()->sendData(data, m_wantEndpointIdZero ? 0 : endpointId);
  return true;
}

} // namespace ndn
