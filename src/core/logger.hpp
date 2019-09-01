#ifndef ESP8266NDN_LOGGER_HPP
#define ESP8266NDN_LOGGER_HPP

#include "logging.hpp"
#include "../vendor/Streaming.h"
#include "../vendor/PriUint64.h"
#include <Printable.h>
#include "detail/fix-maxmin.hpp"

#define LOGGER(module, ...) \
  do { \
    ::ndn::getLogOutput() << _DEC(millis()) << " [" #module "] " << __VA_ARGS__ << "\n"; \
  } while (false)

#endif // ESP8266NDN_LOGGER_HPP
