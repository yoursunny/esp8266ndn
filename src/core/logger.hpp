#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <HardwareSerial.h>
#include "detail/Streaming.h"

#define DBG_PORT Serial

#ifdef DBG_PORT
#define DBG(module, ...) do { DBG_PORT << _DEC(millis()) << " [" #module "] " << __VA_ARGS__ << "\n"; } while (false)
#endif

#endif // LOGGER_HPP