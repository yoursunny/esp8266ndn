#ifndef PRIUINT64_H
#define PRIUINT64_H

#include <stdint.h>
#include <Print.h>
#include <Printable.h>

/** \brief Allow printing uint64_t value.
 *  \code
 *  uint64_t x = 1;
 *  Serial.print(PriUint64<DEC>(x));
 *  \endcode
 */
template<int Base>
class PriUint64 : public Printable
{
public:
  explicit
  PriUint64(uint64_t value, int ignored = 0)
    : m_value(value)
  {
  }

  size_t
  printTo(Print& p) const override
  {
    char buf[8 * sizeof(uint64_t) + 1];
    char* str = &buf[sizeof(buf) - 1];
    *str = '\0';

    uint64_t n = m_value;
    do {
      char c = n % Base;
      n /= Base;

      *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while (n > 0);

    return p.write(str);
  }

private:
  uint64_t m_value;
};

#if defined(ARDUINO_STREAMING) && defined(STREAMING_LIBRARY_VERSION) && STREAMING_LIBRARY_VERSION == 5

/** \brief Print uint64_t as decimal.
 */
inline Print&
operator<<(Print& p, uint64_t x)
{
  return p << PriUint64<DEC>(x);
}

#if defined(ESP8266) || defined(ESP32)
#define PRIUINT64_OVERRIDE_STREAMING_BASED
// <type_traits> is available on ESP8266 and ESP32, but unavailable on AVR.
#endif

#ifdef PRIUINT64_OVERRIDE_STREAMING_BASED
#include <type_traits>

#undef _HEX
#undef _DEC
#undef _OCT
#undef _BIN

class _BASED1 : public _BASED, public Printable
{
public:
  using _BASED::_BASED;

  size_t
  printTo(Print& p) const override
  {
    return p.print(val, base);
  }
};

template<typename V, int Base, typename BaseCls = typename std::conditional<std::is_same<V, uint64_t>::value, PriUint64<Base>, _BASED1>::type>
class _BASED2 : public BaseCls
{
public:
  using BaseCls::BaseCls;
};

#define _HEX(a) (_BASED2<decltype(a),HEX>(a, HEX))
#define _DEC(a) (_BASED2<decltype(a),DEC>(a, DEC))
#define _OCT(a) (_BASED2<decltype(a),OCT>(a, OCT))
#define _BIN(a) (_BASED2<decltype(a),BIN>(a, BIN))

#endif // PRIUINT64_OVERRIDE_STREAMING_BASED
#endif // ARDUINO_STREAMING

#endif // PRIUINT64_H
