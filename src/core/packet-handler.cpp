#include "packet-handler.hpp"

namespace ndn {

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
