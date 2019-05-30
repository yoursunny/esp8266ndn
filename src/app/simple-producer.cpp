#include "simple-producer.hpp"

namespace ndn {

SimpleProducer::Context::Context(Face& face, const InterestLite& interest,
                                 uint64_t endpointId)
  : face(face)
  , interest(interest)
  , endpointId(endpointId)
{
}

ndn_Error
SimpleProducer::Context::sendData(DataLite& data) const
{
  return face.sendData(data, endpointId);
}

ndn_Error
SimpleProducer::Context::sendNack(const NetworkNackLite& nack) const
{
  return face.sendNack(nack, interest, endpointId);
}

SimpleProducer::SimpleProducer(Face& face, const NameLite& prefix, const InterestHandler& handler)
  : PacketHandler(face)
  , m_prefix(prefix)
  , m_handler(handler)
{
}

bool
SimpleProducer::processInterest(const InterestLite& interest, uint64_t endpointId)
{
  if (!m_prefix.match(interest.getName())) {
    return false;
  }

  Context ctx(*getFace(), interest, endpointId);
  return m_handler(ctx, interest);
}

} // namespace ndn
