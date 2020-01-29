#include "nlforwarder.hpp"
#include "detail/nlinit.hpp"

#include "../ndn-lite/forwarder/forwarder.h"

namespace ndn {

void
NlForwarderClass::begin()
{
  detail::nlInitForwarder();
}

void
NlForwarderClass::loop()
{
  ndn_forwarder_process();
}

NlForwarderClass NlForwarder;

} // namespace ndn
