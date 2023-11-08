#ifndef ESP8266NDN_TRANSPORT_BLE_UUID_HPP
#define ESP8266NDN_TRANSPORT_BLE_UUID_HPP

#include <cstdint>

namespace esp8266ndn {

/**
 * @brief BLE service UUID.
 *
 * 099577e3-0788-412a-8824-395084d97391
 */
static const uint8_t BLE_UUID_SVC[] = {0x91, 0x73, 0xd9, 0x84, 0x50, 0x39, 0x24, 0x88,
                                       0x2a, 0x41, 0x88, 0x07, 0xe3, 0x77, 0x95, 0x09};

/**
 * @brief BLE client-to-server communication characteristic UUID.
 *
 * cc5abb89-a541-46d8-a351-2f95a6a81f49
 */
static const uint8_t BLE_UUID_CS[] = {0x49, 0x1f, 0xa8, 0xa6, 0x95, 0x2f, 0x51, 0xa3,
                                      0xd8, 0x46, 0x41, 0xa5, 0x89, 0xbb, 0x5a, 0xcc};

/**
 * @brief BLE server-to-client communication characteristic UUID.
 *
 * 972f9527-0d83-4261-b95d-b1b2fc73bde4
 */
static const uint8_t BLE_UUID_SC[] = {0xe4, 0xbd, 0x73, 0xfc, 0xb2, 0xb1, 0x5d, 0xb9,
                                      0x61, 0x42, 0x83, 0x0d, 0x27, 0x95, 0x2f, 0x97};

} // namespace esp8266ndn

#endif // ESP8266NDN_TRANSPORT_BLE_UUID_HPP
