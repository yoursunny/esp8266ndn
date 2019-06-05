#ifndef ESP8266NDN_QUEUE_HPP
#define ESP8266NDN_QUEUE_HPP

#include <array>
#ifdef ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#endif

namespace ndn {
namespace detail {

/** \brief Generic non-thread-safe queue.
 */
template<typename T, int CAPACITY>
class Queue
{
public:
  bool
  push(T item)
  {
    int newTail = (m_tail + 1) % m_arr.size();
    if (newTail == m_head) {
      return false;
    }

    m_arr[m_tail] = item;
    m_tail = newTail;
    return true;
  }

  std::tuple<T, bool>
  pop()
  {
    if (m_head == m_tail) {
      return std::make_tuple(T(), false);
    }
    T item = m_arr[m_head];
    m_arr[m_head] = {};
    m_head = (m_head + 1) % m_arr.size();
    return std::make_tuple(item, true);
  }

  bool
  isFull() const
  {
    int newTail = (m_tail + 1) % m_arr.size();
    return newTail == m_head;
  }

private:
  std::array<T, CAPACITY> m_arr;
  int m_head = 0;
  int m_tail = 0;
};

#ifdef ESP32

/** \brief Generic thread-safe queue.
 */
template<typename T, int CAPACITY>
class SafeQueue
{
public:
  SafeQueue()
  {
    m_queue = xQueueCreate(CAPACITY, sizeof(T));
  }

  ~SafeQueue()
  {
    vQueueDelete(m_queue);
  }

  bool
  push(T item)
  {
    BaseType_t res = xQueueSendToBack(m_queue, &item, 0);
    return res == pdTRUE;
  }

  std::tuple<T, bool>
  pop()
  {
    T item;
    if (!xQueueReceive(m_queue, &item, 0)) {
      return std::make_tuple(T(), false);
    }
    return std::make_tuple(item, true);
  }

  bool
  isFull() const
  {
    return uxQueueSpacesAvailable(m_queue) == 0;
  }

private:
  QueueHandle_t m_queue;
};

#else // ESP32

template<typename T, int CAPACITY>
using SafeQueue = Queue<T, CAPACITY>;

#endif // ESP32

} // namespace detail
} // namespace ndn

#endif // ESP8266NDN_QUEUE_HPP
