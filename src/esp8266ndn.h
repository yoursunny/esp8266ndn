/** \mainpage esp8266ndn
 *
 *  https://github.com/yoursunny/esp8266ndn/
 */
#ifndef ESP8266NDN_H
#define ESP8266NDN_H

#include "core/detail/fix-maxmin.hpp"

#include "ndn-cpp/ndn-cpp-all.hpp"

#include "app/autoconfig.hpp"
#include "app/ping-client.hpp"
#include "app/ping-server.hpp"
#include "app/rdr-dataset-server.hpp"
#include "app/simple-consumer.hpp"
#include "app/simple-producer.hpp"
#include "app/unix-time.hpp"

#include "core/face.hpp"
#include "core/logging.hpp"
#include "core/packet-buffer.hpp"
#include "core/packet-handler.hpp"
#include "core/uri.hpp"
#include "core/with-components-buffer.hpp"

#include "security/digest-key.hpp"
#include "security/ec-private-key.hpp"
#include "security/ec-public-key.hpp"
#include "security/hmac-key.hpp"
#include "security/private-key.hpp"
#include "security/public-key.hpp"

#include "transport/ble-client-transport.hpp"
#include "transport/ble-server-transport.hpp"
#include "transport/ethernet-transport.hpp"
#include "transport/lite-frag.hpp"
#include "transport/loopback-transport.hpp"
#include "transport/lora-transport.hpp"
#include "transport/transport.hpp"
#include "transport/udp-transport.hpp"

#include "nl/ndnlite.hpp"
#include "nl/nlface.hpp"

#endif // ESP8266NDN_H
