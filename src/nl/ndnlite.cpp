#include "ndnlite.hpp"
#include "nlface.hpp"
#include "detail/nlrng.hpp"

#include "../ndn-lite/encode/key-storage.h"
#include "../ndn-lite/forwarder/forwarder.h"

namespace ndn {

void
NdnLiteClass::begin()
{
  register_platform_security_init(detail::nlRngLoadBackend);
  ndn_key_storage_init();
  ndn_security_init();
  ndn_forwarder_init();
}

void
NdnLiteClass::loop()
{
  ndn_forwarder_process();
}

NdnLiteClass NdnLite;

} // namespace ndn
