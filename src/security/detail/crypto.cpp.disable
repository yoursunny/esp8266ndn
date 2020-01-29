#include "sha256-impl.hpp"
#include "../../ndn-cpp/c/util/crypto.h"
#include <Arduino.h>
#include "../../core/detail/fix-maxmin.hpp"

ndn_Error
ndn_generateRandomBytes(uint8_t* buffer, size_t bufferLength)
{
  // TODO on nRF52 this is not hardware RNG
  for (size_t i = 0; i < bufferLength; ++i) {
    buffer[i] = random(256); // use hardware RNG
  }
  return NDN_ERROR_success;
}

void
ndn_digestSha256(const uint8_t* data, size_t dataLength, uint8_t* digest)
{
  ndn::detail::sha256(data, dataLength, digest);
}
