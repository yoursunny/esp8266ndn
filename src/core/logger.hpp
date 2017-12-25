#ifndef ESP8266NDN_LOGGER_HPP
#define ESP8266NDN_LOGGER_HPP

#include "logging.hpp"
#include "detail/Streaming.h"

#define DBG(module, ...) \
  do { \
    ::ndn::getLogOutput() << _DEC(millis()) << " [" #module "] " << __VA_ARGS__ << "\n"; \
  } while (false)

#endif // ESP8266NDN_LOGGER_HPP
