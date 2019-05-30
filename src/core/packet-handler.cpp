#include "packet-handler.hpp"
#include "face.hpp"

namespace ndn {

PacketHandler::PacketHandler() = default;

PacketHandler::PacketHandler(Face& face, int8_t prio)
{
  face.addHandler(this, prio);
}

PacketHandler::~PacketHandler()
{
  if (m_face != nullptr) {
    m_face->removeHandler(this);
  }
}

bool
PacketHandler::processInterest(const InterestLite& interest, uint64_t endpointId)
{
  return false;
}

bool
PacketHandler::processData(const DataLite& data, uint64_t endpointId)
{
  return false;
}

bool
PacketHandler::processNack(const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId)
{
  return false;
}

} // namespace ndn
