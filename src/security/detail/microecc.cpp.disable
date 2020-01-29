#include "microecc.hpp"
#include "../../ndn-cpp/lite/util/crypto-lite.hpp"

namespace ndn {
namespace detail {

class MicroEccRng
{
public:
  MicroEccRng()
  {
    uECC_set_rng(&rng);
  }

private:
  static int
  rng(uint8_t* dest, unsigned size)
  {
    CryptoLite::generateRandomBytes(dest, size);
    return 1;
  }
};

void
setMicroEccRng()
{
  static MicroEccRng instance;
}

} // namespace detail
} // namespace ndn
