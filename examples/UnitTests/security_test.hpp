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
              F("EC41C17185C911F9F2FED2A8F6CC9E1AB4E9F06C7E62DD2E4BAEFC5B56596043"));
  assertTrue(this->verify(key));
}

testF(KeyFixtureOnce, HmacKey_basic)
{
  ndn::HmacKey key(reinterpret_cast<const uint8_t*>("secret"), 6);
  assertEqual(key.getMaxSigLength(), ndn_SHA256_DIGEST_SIZE);
  assertEqual(this->sign(key), static_cast<int>(ndn_SHA256_DIGEST_SIZE));
  // echo -n 'yoursunny.com' | openssl sha256 -hmac 'secret'
  assertEqual(this->getSigHex(),
              F("E6680A517902FF958545AF733359499BDC59FAB051FCFAC02A0F253C099DA12E"));
  assertTrue(this->verify(key));
}
