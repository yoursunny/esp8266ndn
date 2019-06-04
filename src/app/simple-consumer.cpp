#include "simple-consumer.hpp"
#include <Arduino.h>
#include "../core/detail/fix-maxmin.hpp"

namespace ndn {

SimpleConsumer::SimpleConsumer(Face& face, InterestLite& interest, int timeout)
  : PacketHandler(face)
  , interest(interest)
  , m_timeoutDuration(timeout)
  , m_timeoutAt(0)
  , m_result(Result::NONE)
  , m_pb(nullptr)
{
}

SimpleConsumer::~SimpleConsumer()
{
  if (m_pb != nullptr) {
    delete m_pb;
  }
}

ndn_Error
SimpleConsumer::sendInterest()
{
  this->prepareSendInterest();
  return getFace()->sendInterest(interest);
}

ndn_Error
SimpleConsumer::sendSignedInterest()
{
  this->prepareSendInterest();
  return getFace()->sendSignedInterest(interest);
}

void
SimpleConsumer::prepareSendInterest()
{
  m_result = Result::NONE;
  m_timeoutAt = millis() + m_timeoutDuration;
}

SimpleConsumer::Result
SimpleConsumer::getResult() const
{
  if (m_result == Result::NONE && m_timeoutAt > 0 && millis() >= m_timeoutAt) {
    const_cast<SimpleConsumer*>(this)->m_result = Result::TIMEOUT;
  }
  return m_result;
}

SimpleConsumer::Result
SimpleConsumer::waitForResult() const
{
  while (this->getResult() == Result::NONE) {
    getFace()->loop();
    delay(1);
  }
  return this->getResult();
}

const DataLite*
SimpleConsumer::getData() const
{
  if (m_result != Result::DATA) {
    return nullptr;
  }
  return m_pb->getData();
}

const NetworkNackLite*
SimpleConsumer::getNack() const
{
  if (m_result != Result::NACK) {
    return nullptr;
  }
  return m_pb->getNack();
}

uint64_t
SimpleConsumer::getEndpointId() const
{
  if (m_pb == nullptr) {
    return 0;
  }
  return m_pb->endpointId;
}

bool
SimpleConsumer::processData(const DataLite& data, uint64_t endpointId)
{
  if (!interest.getName().match(data.getName())) {
    return false;
  }

  m_result = Result::DATA;
  m_pb = getFace()->swapPacketBuffer(m_pb);
  m_pb->endpointId = endpointId;
  return true;
}

bool
SimpleConsumer::processNack(const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId)
{
  if (!this->interest.getName().equals(interest.getName())) {
    return false;
  }

  m_result = Result::NACK;
  m_pb = getFace()->swapPacketBuffer(m_pb);
  m_pb->endpointId = endpointId;
  return true;
}

} // namespace ndn
