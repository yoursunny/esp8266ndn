#include "logging.hpp"

namespace ndn {

class NullPrint : public Print
{
public:
  size_t write(uint8_t) override
  {
    return 1;
  }
};

class LogOutputWrapper
{
public:
  LogOutputWrapper()
    : output(&nullPrint)
  {}

public:
  NullPrint nullPrint;
  Print* output;
};

static LogOutputWrapper&
getLogOutputWrapper()
{
  static LogOutputWrapper w;
  return w;
}

Print&
getLogOutput()
{
  return *getLogOutputWrapper().output;
}

void
setLogOutput(Print& output)
{
  getLogOutputWrapper().output = &output;
}

PrintHex::PrintHex(const uint8_t* buf, size_t len)
  : m_buf(buf)
  , m_len(len)
{}

size_t
PrintHex::printTo(Print& p) const
{
  for (size_t i = 0; i < m_len; ++i) {
    p.printf("%02x", m_buf[i]);
  }
  return 2 * m_len;
}

} // namespace ndn
