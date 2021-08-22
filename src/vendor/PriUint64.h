#ifndef PRIUINT64_H
#define PRIUINT64_H

#include <Print.h>
#include <Printable.h>
#include <stdint.h>

namespace priuint64 {
namespace detail {

inline size_t
print(Print& p, uint64_t n, uint64_t base)
{
  char buf[8 * sizeof(uint64_t) + 1];
  char* str = &buf[sizeof(buf) - 1];
  *str = '\0';

  do {
    char c = n % base;
    n /= base;

    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while (n > 0);

  return p.write(str);
}

} // namespace detail

/**
 * @brief Allow printing uint64_t value.
 * @code
 * uint64_t x = 1;
 * Serial.print(PriUint64<DEC>(x));
 * @endcode
 */
template<uint64_t base>
class PriUint64 : public Printable
{
public:
  explicit PriUint64(uint64_t value)
    : m_value(value)
  {}

  size_t printTo(Print& p) const override
  {
    return detail::print(p, m_value, base);
  }

private:
  uint64_t m_value;
};

} // namespace priuint64

using priuint64::PriUint64;

#if defined(ARDUINO_STREAMING) && STREAMING_LIBRARY_VERSION == 6

/** @brief Print uint64_t as decimal. */
inline Print&
operator<<(Print& p, uint64_t x)
{
  return p << PriUint64<DEC>(x);
}

template<>
inline Print&
operator<<(Print& obj, const _BASED<uint64_t>& arg)
{
  priuint64::detail::print(obj, arg.val, static_cast<uint64_t>(arg.base));
  return obj;
}

#endif // ARDUINO_STREAMING

#endif // PRIUINT64_H
