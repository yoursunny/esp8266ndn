#ifndef ESP8266NDN_PORT_QUEUE_FREERTOS_HPP
#define ESP8266NDN_PORT_QUEUE_FREERTOS_HPP

#include "choose.hpp"

#include <cstdlib>
#include <tuple>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace esp8266ndn {
namespace ndnph_port_freertos {

/** @brief Generic thread-safe queue, implemented with FreeRTOS queue API. */
template<typename T, size_t capacity>
class SafeQueue
{
public:
  using Item = T;

  SafeQueue()
  {
    m_queue = xQueueCreate(capacity, sizeof(Item));
  }

  ~SafeQueue()
  {
    vQueueDelete(m_queue);
  }

  bool push(Item item)
  {
    BaseType_t res = xQueueSendToBack(m_queue, &item, 0);
    return res == pdTRUE;
  }

  std::tuple<Item, bool> pop()
  {
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
