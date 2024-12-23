#ifndef ESP8266NDN_PORT_QUEUE_FREERTOS_HPP
#define ESP8266NDN_PORT_QUEUE_FREERTOS_HPP

#include "choose.h"

#include <cstdlib>
#include <tuple>

#if defined(ARDUINO_ARCH_RP2040)
#include <Arduino.h> // https://github.com/earlephilhower/arduino-pico/issues/2287
#include <FreeRTOS.h>
#include <queue.h>
#else
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#endif

namespace esp8266ndn {
namespace ndnph_port_freertos {

/** @brief Generic thread-safe queue, implemented with FreeRTOS queue API. */
template<typename T, size_t capacity>
class SafeQueue {
public:
  using Item = T;
  static_assert(std::is_trivially_copyable<Item>::value, "");
  static_assert(std::is_trivially_destructible<Item>::value, "");

  SafeQueue() {
    m_queue = xQueueCreate(capacity, sizeof(Item));
  }

  ~SafeQueue() {
    vQueueDelete(m_queue);
  }

  bool push(Item item) {
    BaseType_t res = xQueueSendToBack(m_queue, &item, 0);
    return res == pdTRUE;
  }

  std::tuple<Item, bool> pop() {
    Item item;
    BaseType_t res = xQueueReceive(m_queue, &item, 0);
    return std::make_tuple(std::move(item), res == pdTRUE);
  }

private:
  QueueHandle_t m_queue;
};

} // namespace ndnph_port_freertos
} // namespace esp8266ndn

#ifdef ESP8266NDN_PORT_QUEUE_FREERTOS
namespace ndnph {
namespace port {
template<typename T, size_t capacity>
using SafeQueue = esp8266ndn::ndnph_port_freertos::SafeQueue<T, capacity>;
} // namespace port
} // namespace ndnph
#endif // ESP8266NDN_PORT_QUEUE_FREERTOS

#endif // ESP8266NDN_PORT_QUEUE_FREERTOS_HPP
