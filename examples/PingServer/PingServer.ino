// PingServer example currently does not perform prefix registration.
// It's necessary to manually add a route toward the ESP8266 on the router.

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";
const char* NDN_ROUTER_HOST = "my-server.example.org";
const uint16_t NDN_ROUTER_PORT = 6363;
const uint8_t NDN_HMAC_KEY[] = {
  0xaf, 0x4a, 0xb1, 0xd2, 0x52, 0x02, 0x7d, 0x67, 0x7d, 0x85, 0x14, 0x31, 0xf1, 0x0e, 0x0e, 0x1d,
  0x92, 0xa9, 0xd4, 0x0a, 0x0f, 0xf4, 0x49, 0x90, 0x06, 0x7e, 0xf6, 0x50, 0xc8, 0x50, 0x2c, 0x6b,
  0x1e, 0xbe, 0x00, 0x2d, 0x5c, 0xaf, 0xd9, 0xe1, 0xd3, 0xa5, 0x25, 0xe2, 0x72, 0xfb, 0xa7, 0xa7,
  0xe4, 0xb0, 0xc9, 0x00, 0xc2, 0xfe, 0x58, 0xb4, 0x9f, 0x38, 0x0b, 0x45, 0xc9, 0x30, 0xfe, 0x26
};
char PREFIX0[] = "/example/esp8266-A/ping"; // must be mutable
char PREFIX1[] = "/example/esp8266-B/ping";

WiFiUDP g_udp;
ndn::UnicastUdpTransport g_transport(g_udp);
ndn::Face g_face(g_transport);

ndn_NameComponent g_comps0[8];
ndn::NameLite g_prefix0(g_comps0, 8);
ndn::PingServer g_server0(g_face, g_prefix0);
ndn_NameComponent g_comps1[8];
ndn::NameLite g_prefix1(g_comps1, 8);
ndn::PingServer g_server1(g_face, g_prefix1);

bool
replyNoRouteNack(const ndn::InterestLite& interest)
{
  ndn::NetworkNackLite nack;
  nack.setReason(ndn_NetworkNackReason_NO_ROUTE);
  g_face.sendNack(nack, interest);
  Serial.print("<N ");
  Serial.println(ndn::PrintUri(interest.getName()));
  return true;
}

void
processInterest(void*, const ndn::InterestLite& interest, uint64_t)
{
  Serial.print(">I ");
  Serial.println(ndn::PrintUri(interest.getName()));
  g_server0.processInterest(interest) ||
  g_server1.processInterest(interest) ||
  replyNoRouteNack(interest);
}

void
makePayload(void* arg, const ndn::InterestLite& interest, uint8_t* payloadBuf, size_t* payloadSize)
{
  auto text = reinterpret_cast<const char*>(arg);
  size_t len = strlen(text);
  memcpy(payloadBuf, text, len);
  *payloadSize = len;
  Serial.print("<D ");
  Serial.println(ndn::PrintUri(interest.getName()));
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  delay(1000);

  IPAddress routerIp;
  if (!WiFi.hostByName(NDN_ROUTER_HOST, routerIp)) {
    Serial.println("cannot resolve router IP");
    ESP.restart();
  }
  g_transport.begin(routerIp, NDN_ROUTER_PORT, 6363);
  g_face.onInterest(&processInterest, nullptr);
  g_face.setHmacKey(NDN_HMAC_KEY, sizeof(NDN_HMAC_KEY));

  ndn::parseNameFromUri(g_prefix0, PREFIX0);
  ndn::parseNameFromUri(g_prefix1, PREFIX1);

  g_server0.onProbe(&makePayload, const_cast<void*>(reinterpret_cast<const void*>("PingServer 0")));
  g_server1.onProbe(&makePayload, const_cast<void*>(reinterpret_cast<const void*>("PingServer 1")));

  // express one Interest to create face on router
  ndn::InterestLite interest(nullptr, 0, nullptr, 0, nullptr, 0);
  g_face.sendInterest(interest);
}

void
loop()
{
  g_face.loop();
  delay(10);
}
