#include "ping-server.hpp"
#include "../core/logger.hpp"
#include "../core/uri.hpp"

#define PINGSERVER_DBG(...) DBG(PingServer, __VA_ARGS__)

namespace ndn {

// have a separate Handler class because deprecated public processInterest has same parameters
class PingServer::Handler : public PacketHandler
{
public:
  explicit
  Handler(PingServer& pingServer)
    : m_pingServer(pingServer)
  {
  }

private:
  bool
  processInterest(const InterestLite& interest, uint64_t endpointId) override
  {
    return m_pingServer.doProcessInterest(interest, endpointId);
  }

private:
  PingServer& m_pingServer;
};

PingServer::PingServer(Face& face, const NameLite& prefix)
  : m_face(face)
  , m_prefix(prefix)
  , m_probeCb(nullptr)
  , m_wantEndpointIdZero(false)
{
  m_handler = new Handler(*this);
  m_face.addHandler(m_handler);
}

PingServer::~PingServer()
{
  m_face.removeHandler(m_handler);
  delete m_handler;
}

bool
PingServer::doProcessInterest(const InterestLite& interest, uint64_t endpointId)
{
  if (!m_prefix.match(interest.getName())) {
    return false;
  }

  static ndn_NameComponent nameComps[NDNPINGSERVER_NAMECOMPS_MAX];
  ndn::DataLite data(nameComps, NDNPINGSERVER_NAMECOMPS_MAX, nullptr, 0);
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
  m_face.sendData(data, m_wantEndpointIdZero ? 0 : endpointId);
  return true;
}

} // namespace ndn
