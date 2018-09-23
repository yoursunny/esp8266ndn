#include "test-common.hpp"

class KeyFixture : public TestOnce
{
public:
  static std::pair<const uint8_t*, size_t>
  getInput()
  {
    static const uint8_t THE_INPUT[] {
      0x79, 0x6F, 0x75, 0x72, 0x73, 0x75, 0x6E, 0x6E, 0x79, 0x2E, 0x63, 0x6F, 0x6D};
    return {THE_INPUT, sizeof(THE_INPUT)};
  }

  int
  sign(const ndn::PrivateKey& key)
  {
    const uint8_t* input;
    size_t inputLen;
    std::tie(input, inputLen) = getInput();

    sig.resize(key.getMaxSigLength());
    int sigLen = key.sign(input, inputLen, sig.data());
    sig.resize(sigLen);
    return sigLen;
  }

  String
  getSigHex() const
  {
    return toHexString(sig.data(), sig.size());
  }

  bool
  verify(const ndn::PublicKey& key) const
  {
    const uint8_t* input;
    size_t inputLen;
    std::tie(input, inputLen) = getInput();

    return key.verify(input, inputLen, sig.data(), sig.size());
  }

  void
  modifySig()
  {
    if (sig.empty()) {
      return;
    }
    size_t pos = sig.size() / 2;
    sig[pos] += 1;
  }

  void
  teardown() override
  {
    std::vector<uint8_t> empty;
    sig.swap(empty);
  }

public:
  std::vector<uint8_t> sig;
};

testF(KeyFixture, DigestKey_basic)
{
  ndn::DigestKey key;
  assertEqual(key.getMaxSigLength(), ndn_SHA256_DIGEST_SIZE);
  assertEqual(this->sign(key), static_cast<int>(ndn_SHA256_DIGEST_SIZE));
  // echo -n 'yoursunny.com' | openssl sha256
  assertEqual(this->getSigHex(),
              F("EC41C17185C911F9F2FED2A8F6CC9E1AB4E9F06C7E62DD2E4BAEFC5B56596043"));
  assertTrue(this->verify(key));

  this->modifySig();
  assertFalse(this->verify(key));
}

testF(KeyFixture, HmacKey_basic)
{
  ndn::HmacKey key(reinterpret_cast<const uint8_t*>("secret"), 6);
  assertEqual(key.getMaxSigLength(), ndn_SHA256_DIGEST_SIZE);
  assertEqual(this->sign(key), static_cast<int>(ndn_SHA256_DIGEST_SIZE));
  // echo -n 'yoursunny.com' | openssl sha256 -hmac 'secret'
  assertEqual(this->getSigHex(),
              F("E6680A517902FF958545AF733359499BDC59FAB051FCFAC02A0F253C099DA12E"));
  assertTrue(this->verify(key));

  this->modifySig();
  assertFalse(this->verify(key));
}

const uint8_t EcKey_basic_PVT[] PROGMEM {
  0xD7, 0xCB, 0xA5, 0xF4, 0xC6, 0x89, 0x3E, 0x56, 0x2B, 0xFA, 0xD1, 0x52, 0x8A, 0x8A, 0xCD, 0x4C,
  0x29, 0x3C, 0xCA, 0xC1, 0x37, 0x7F, 0x87, 0x90, 0x9A, 0x14, 0x6E, 0xA0, 0x15, 0xD8, 0x88, 0x6E,
};
const uint8_t EcKey_basic_PUB[] PROGMEM {   0x04,
  0x35, 0xF3, 0xE6, 0x92, 0x49, 0x13, 0x58, 0x46, 0xDB, 0xDA, 0x83, 0x68, 0xEA, 0x4D, 0x89, 0xD0,
  0xA0, 0x6B, 0x6C, 0xAE, 0xD4, 0xBC, 0x70, 0x41, 0x00, 0x41, 0x71, 0xA8, 0x72, 0xAB, 0x41, 0x2C,
  0x86, 0x22, 0xB5, 0x29, 0x94, 0xE1, 0x96, 0x8C, 0x3A, 0x5E, 0xC6, 0x79, 0x49, 0x2E, 0x3F, 0xF8,
  0x86, 0x56, 0xAF, 0x15, 0xEB, 0x7B, 0x82, 0x73, 0xAA, 0xB7, 0x8F, 0x34, 0xEA, 0x42, 0x35, 0xFD,
};
testF(KeyFixture, EcKey_basic)
{
  ndn::NameWCB<1> name;
  name.append("key");
  ndn::EcPrivateKey pvt(name);
  ndn::EcPublicKey pub;

  assertLessOrEqual(this->sign(pvt), 0);
  assertTrue(pvt.import(EcKey_basic_PVT));
  assertMore(this->sign(pvt), 0);
  yield();

  assertFalse(this->verify(pub));
  assertTrue(pub.import(EcKey_basic_PUB));
  assertTrue(this->verify(pub));
  yield();

  this->modifySig();
  assertFalse(this->verify(pub));
}

testF(KeyFixture, EcKey_generate)
{
  ndn::NameWCB<1> name;
  name.append("key");
  ndn::EcPrivateKey pvt(name);
  ndn::EcPublicKey pub;
  assertTrue(pvt.generate(pub));
  yield();

  assertMore(this->sign(pvt), 0);
  yield();
  assertTrue(this->verify(pub));
  yield();
  this->modifySig();
  assertFalse(this->verify(pub));
}
