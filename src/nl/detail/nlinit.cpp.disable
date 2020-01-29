#include "nlinit.hpp"

#include "../../ndn-cpp/lite/util/crypto-lite.hpp"

#include "../../ndn-lite/encode/key-storage.h"
#include "../../ndn-lite/forwarder/forwarder.h"
#include "../../ndn-lite/security/ndn-lite-rng.h"
#include "../../ndn-lite/security/ndn-lite-sec-config.h"

namespace ndn {
namespace detail {

class NlSecurityInit
{
public:
  NlSecurityInit()
  {
    register_platform_security_init(loadRngBackend);
    ndn_security_init();
  }

private:
  static int
  rng(uint8_t* dest, unsigned size)
  {
    CryptoLite::generateRandomBytes(dest, size);
    return 1;
  }

  static void
  loadRngBackend()
  {
    ndn_rng_backend_t* backend = ndn_rng_get_backend();
    backend->rng = rng;
  }
};

void
nlInitSecurity()
{
  static NlSecurityInit init;
}

class NlKeyStorageInit
{
public:
  NlKeyStorageInit()
  {
    ndn_key_storage_init();
  }
};

void
nlInitKeyStorage()
{
  static NlKeyStorageInit init;
}

class NlForwarderInit
{
public:
  NlForwarderInit()
  {
    ndn_forwarder_init();
  }
};

void
nlInitForwarder()
{
  nlInitSecurity();
  static NlForwarderInit init;
}

} // namespace detail
} // namespace ndn
