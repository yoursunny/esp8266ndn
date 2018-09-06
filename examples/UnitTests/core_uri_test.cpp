#include "test-common.hpp"

class UriFixture : public TestOnce
{
public:
  bool
  parse(const __FlashStringHelper* input)
  {
    m_buf.resize(strlen_P(reinterpret_cast<const char*>(input)) + 1);
    memcpy_P(m_buf.data(), input, m_buf.size());
    return ndn::parseNameFromUri(name, m_buf.data());
  }

  String
  getCompHex(int i) const
  {
    const ndn::BlobLite& compV = name.get(i).getValue();
    return toHexString(compV.buf(), compV.size());
  }

  String
  toString() const
  {
    StreamString os;
    os.print(ndn::PrintUri{name});
    return os;
  }

public:
  ndn::NameWCB<4> name;

private:
  std::vector<char> m_buf;
};

testF(UriFixture, Uri_empty)
{
  auto input = F("/");
  assertTrue(this->parse(input));
  assertEqual(name.size(), 0U);
  assertEqual(this->toString(), input);

  assertTrue(this->parse(F("ndn:/")));
  assertEqual(name.size(), 0U);
  assertEqual(this->toString(), input);
}

testF(UriFixture, Uri_simple)
{
  auto input = F("/A/B/C");
  assertTrue(this->parse(input));
  assertEqual(name.size(), 3U);
  assertEqual(this->getCompHex(0), "41");
  assertEqual(this->getCompHex(1), "42");
  assertEqual(this->getCompHex(2), "43");
  assertEqual(this->toString(), input);
}

testF(UriFixture, Uri_dots)
{
  auto input = F("/.../..../...../...A...");
  assertTrue(this->parse(input));
  assertEqual(name.size(), 4U);
  assertEqual(this->getCompHex(0), "");
  assertEqual(this->getCompHex(1), "2E");
  assertEqual(this->getCompHex(2), "2E2E");
  assertEqual(this->getCompHex(3), "2E2E2E412E2E2E");
  assertEqual(this->toString(), input);
}

testF(UriFixture, Uri_percent)
{
  auto input = F("/%00GH%ab%cD%EF/%2B-._~");
  assertTrue(this->parse(input));
  assertEqual(name.size(), 2U);
  assertEqual(this->getCompHex(0), "004748ABCDEF");
  assertEqual(this->getCompHex(1), "2B2D2E5F7E");

  String uri(input);
  uri.toUpperCase();
  assertEqual(this->toString(), uri);
}
