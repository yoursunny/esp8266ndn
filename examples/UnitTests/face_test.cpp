#include "test-common.hpp"

class FaceFixture : public TestOnce
{
public:
  FaceFixture()
    : faceA(transportA)
    , faceB(transportB)
  {
    transportA.begin(transportB);
    faceA.setSigningKey(key);
    faceB.setSigningKey(key);
    faceA.enableTracing(Serial, "A");
    faceB.enableTracing(Serial, "B");
  }

  void
  loops()
  {
    faceA.loop();
    faceB.loop();
  }

public:
  ndn::DigestKey key;
  ndn::LoopbackTransport transportA;
  ndn::LoopbackTransport transportB;
  ndn::Face faceA;
  ndn::Face faceB;
};

testF(FaceFixture, Face_InterestData)
{
  ndn::NameWCB<1> prefix;
  prefix.append("A");
  ndn::SimpleProducer producer(faceA, prefix,
    [] (ndn::SimpleProducer::Context& ctx, const ndn::InterestLite& interest) {
      ndn::DataWCB<2, 0> data;
      data.setName(interest.getName());
      ctx.sendData(data);
      return true;
    });

  ndn::InterestWCB<2, 0> interest;
  interest.getName().append("A");
  interest.getName().append("1");
  ndn::SimpleConsumer consumer(faceB, interest);
  consumer.sendInterest();

  while (consumer.getResult() == ndn::SimpleConsumer::Result::NONE) {
    this->loops();
  }
  assertEqual(static_cast<int>(consumer.getResult()), static_cast<int>(ndn::SimpleConsumer::Result::DATA));
}

testF(FaceFixture, Face_InterestNack)
{
  ndn::NameWCB<1> prefix;
  prefix.append("A");
  ndn::SimpleProducer producer(faceA, prefix,
    [] (ndn::SimpleProducer::Context& ctx, const ndn::InterestLite& interest) {
      ndn::NetworkNackLite nack;
      nack.setReason(ndn_NetworkNackReason_CONGESTION);
      ctx.sendNack(nack);
      return true;
    });

  ndn::InterestWCB<2, 0> interest;
  interest.getName().append("A");
  interest.getName().append("1");
  ndn::SimpleConsumer consumer(faceB, interest);
  consumer.sendInterest();

  while (consumer.getResult() == ndn::SimpleConsumer::Result::NONE) {
    this->loops();
  }
  assertEqual(static_cast<int>(consumer.getResult()), static_cast<int>(ndn::SimpleConsumer::Result::NACK));
}
