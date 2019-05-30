#ifndef ESP8266NDN_SIMPLE_PRODUCER_HPP
#define ESP8266NDN_SIMPLE_PRODUCER_HPP

#include "../core/face.hpp"

namespace ndn {

/** \brief Allow developing a simple producer using procedural programming.
 */
class SimpleProducer : public PacketHandler
{
public:
  class Context
  {
  public:
    ndn_Error
    sendData(DataLite& data) const;

    ndn_Error
    sendNack(const NetworkNackLite& nack) const;

  private:
    Context(Face& face, const InterestLite& interest, uint64_t endpointId);

    friend class SimpleProducer;

  public:
    Face& face;
    const InterestLite& interest;
    uint64_t endpointId;
  };

  /** \brief Interest handler
   */
  typedef bool (*InterestHandler)(Context& ctx, const InterestLite& interest);

  SimpleProducer(Face& face, const NameLite& prefix, const InterestHandler& handler);

private:
  bool
  processInterest(const InterestLite& interest, uint64_t endpointId) override;

private:
  const NameLite& m_prefix;
  InterestHandler m_handler;
};

} // namespace ndn

#endif // ESP8266NDN_SIMPLE_PRODUCER_HPP
