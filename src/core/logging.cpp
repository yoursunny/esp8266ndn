#include "logging.hpp"

namespace ndn {

class NullPrint : public Print
{
public:
  size_t
  write(uint8_t) override
  {
    return 1;
  }
};

class LogOutputWrapper
{
public:
  LogOutputWrapper()
    : output(&nullPrint)
  {
  }

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

} // namespace ndn
