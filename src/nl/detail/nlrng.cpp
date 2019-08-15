#include "nlrng.hpp"

#include "../../ndn-cpp/lite/util/crypto-lite.hpp"

#include "../../ndn-lite/security/ndn-lite-rng.h"
#include "../../ndn-lite/security/ndn-lite-sec-config.h"

namespace ndn {
namespace detail {

static int
rng(uint8_t* dest, unsigned size)
{
  CryptoLite::generateRandomBytes(dest, size);
  return 1;
}

void
nlRngLoadBackend()
{
  ndn_rng_backend_t* backend = ndn_rng_get_backend();
  backend->rng = rng;
}

} // namespace detail
} // namespace ndn
