#ifndef ESP8266NDN_SIMPLE_CONSUMER_HPP
#define ESP8266NDN_SIMPLE_CONSUMER_HPP

#include "../core/face.hpp"

namespace ndn {

/** \brief Allow developing a simple consumer using procedural programming.
 *  \note This API is not yet finalized and may change without notice.
 */
class SimpleConsumer : public PacketHandler
{
public:
  SimpleConsumer(Face& face, InterestLite& interest, int timeout = 1000);

  ~SimpleConsumer();

  ndn_Error
  sendInterest();

  ndn_Error
  sendSignedInterest();

  enum class Result {
    NONE    = 0,
    TIMEOUT = -1,
    DATA    = 0x05,
    NACK    = 0x0320,
  };

  Result
  getResult() const;

  Result
  waitForResult() const;

  const DataLite*
  getData() const;

  const NetworkNackLite*
  getNack() const;

  uint64_t
  getEndpointId() const
  {
    return m_endpointId;
  }

  bool
  verify(const PublicKey& pubKey) const;

private:
  void
  prepareSendInterest();

  bool
  processData(const DataLite& data, uint64_t endpointId) override;

  bool
  processNack(const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId) override;

public:
  InterestLite& interest;

private:
  Face& m_face;
  int m_timeoutDuration;
  unsigned long m_timeoutAt;
  Result m_result;
  PacketBuffer* m_pb;
  uint64_t m_endpointId;
};

} // namespace ndn

#endif // ESP8266NDN_SIMPLE_CONSUMER_HPP
