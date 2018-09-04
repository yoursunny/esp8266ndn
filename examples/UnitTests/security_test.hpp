#include <StreamString.h>

class KeyFixtureBase
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

private:
  static String
  toHexString(const uint8_t* input, size_t len)
  {
    StreamString os;
    for (size_t i = 0; i < len; ++i) {
      os.printf("%02x", input[i]);
    }
    return os;
  }

public:
  std::vector<uint8_t> sig;
};

class KeyFixtureOnce : public TestOnce, public KeyFixtureBase
{
};

testF(KeyFixtureOnce, DigestKey_basic)
{
  ndn::DigestKey key;
  assertEqual(key.getMaxSigLength(), ndn_SHA256_DIGEST_SIZE);
  assertEqual(this->sign(key), static_cast<int>(ndn_SHA256_DIGEST_SIZE));
  // echo -n 'yoursunny.com' | openssl sha256
  assertEqual(this->getSigHex(),
              F("ec41c17185c911f9f2fed2a8f6cc9e1ab4e9f06c7e62dd2e4baefc5b56596043"));
  assertTrue(this->verify(key));
}

testF(KeyFixtureOnce, HmacKey_basic)
{
  ndn::HmacKey key(reinterpret_cast<const uint8_t*>("secret"), 6);
  assertEqual(key.getMaxSigLength(), ndn_SHA256_DIGEST_SIZE);
  assertEqual(this->sign(key), static_cast<int>(ndn_SHA256_DIGEST_SIZE));
  // echo -n 'yoursunny.com' | openssl sha256 -hmac 'secret'
  assertEqual(this->getSigHex(),
              F("e6680a517902ff958545af733359499bdc59fab051fcfac02a0f253c099da12e"));
  assertTrue(this->verify(key));
}
