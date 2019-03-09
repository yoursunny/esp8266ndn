#ifndef ESP8266NDN_LOGGER_HPP
#define ESP8266NDN_LOGGER_HPP

#include "logging.hpp"
#include "detail/Streaming.h"
#include "detail/PriUint64.h"
#include "detail/fix-maxmin.hpp"

#define DBG(module, ...) \
  do { \
    ::ndn::getLogOutput() << _DEC(millis()) << " [" #module "] " << __VA_ARGS__ << "\n"; \
  } while (false)

#endif // ESP8266NDN_LOGGER_HPP
