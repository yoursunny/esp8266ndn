#include "test-common.hpp"

class FaceFixture : public TestOnce
{
public:
  void
  setup() override
  {
    TestOnce::setup();
    key = new ndn::DigestKey();
    transportA = new ndn::LoopbackTransport();
    transportB = new ndn::LoopbackTransport();
    transportA->begin(*transportB);
    faceA = makeFace(transportA, "A");
    faceB = makeFace(transportB, "A");
  }

  void
  loops()
  {
    faceA->loop();
    faceB->loop();
  }

  void
  teardown() override
  {
    delete faceA;
    delete faceB;
    delete transportA;
    delete transportB;
    delete key;
    TestOnce::teardown();
  }

private:
  ndn::Face*
  makeFace(ndn::Transport* transport, const String& tracingPrefix)
  {
    auto face = new ndn::Face(*transport);
    face->setSigningKey(*key);
    face->enableTracing(Serial, tracingPrefix);
    face->addReceiveBuffers(2);
    return face;
  }

public:
  ndn::DigestKey* key;
  ndn::LoopbackTransport* transportA;
  ndn::LoopbackTransport* transportB;
  ndn::Face* faceA;
  ndn::Face* faceB;
};

testF(FaceFixture, Face_InterestData)
{
  ndn::NameWCB<1> prefix;
  prefix.append("A");
  ndn::SimpleProducer producer(*faceA, prefix,
    [] (ndn::SimpleProducer::Context& ctx, const ndn::InterestLite& interest) {
      ndn::DataWCB<2, 0> data;
      data.setName(interest.getName());
      ctx.sendData(data);
      return true;
    });

  ndn::InterestWCB<2, 0> interest;
  interest.getName().append("A");
  interest.getName().append("1");
  ndn::SimpleConsumer consumer(*faceB, interest);
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
  ndn::SimpleProducer producer(*faceA, prefix,
    [] (ndn::SimpleProducer::Context& ctx, const ndn::InterestLite& interest) {
      ndn::NetworkNackLite nack;
      nack.setReason(ndn_NetworkNackReason_CONGESTION);
      ctx.sendNack(nack);
      return true;
    });

  ndn::InterestWCB<2, 0> interest;
  interest.getName().append("A");
  interest.getName().append("1");
  ndn::SimpleConsumer consumer(*faceB, interest);
  consumer.sendInterest();

  while (consumer.getResult() == ndn::SimpleConsumer::Result::NONE) {
    this->loops();
  }
  assertEqual(static_cast<int>(consumer.getResult()), static_cast<int>(ndn::SimpleConsumer::Result::NACK));
}
