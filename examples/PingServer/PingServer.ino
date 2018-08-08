#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

// Define interface names for MulticastEthernetTransport.
// In case they are wrong, have a look at MulticastEthernetTransport::listNetifs output.
#if defined(ESP8266)
// lwip v1.4 STA interface "ew" 0
#define ETHER_FACE_IFNAME "ew"
#define ETHER_FACE_IFNUM  0
// lwip v2.0 is not supported; its STA interface name is "st" 0
#elif defined(ESP32)
// STA interface "st" 1
#define ETHER_FACE_IFNAME "st"
#define ETHER_FACE_IFNUM  1
#endif

const uint8_t HMAC_KEY[] = {
  0xaf, 0x4a, 0xb1, 0xd2, 0x52, 0x02, 0x7d, 0x67, 0x7d, 0x85, 0x14, 0x31, 0xf1, 0x0e, 0x0e, 0x1d,
  0x92, 0xa9, 0xd4, 0x0a, 0x0f, 0xf4, 0x49, 0x90, 0x06, 0x7e, 0xf6, 0x50, 0xc8, 0x50, 0x2c, 0x6b,
  0x1e, 0xbe, 0x00, 0x2d, 0x5c, 0xaf, 0xd9, 0xe1, 0xd3, 0xa5, 0x25, 0xe2, 0x72, 0xfb, 0xa7, 0xa7,
  0xe4, 0xb0, 0xc9, 0x00, 0xc2, 0xfe, 0x58, 0xb4, 0x9f, 0x38, 0x0b, 0x45, 0xc9, 0x30, 0xfe, 0x26
};
char PREFIX0[] = "/example/esp8266/ethernet/ping";
char PREFIX1[] = "/example/esp8266/udp/ping";
// these prefixes must be mutable, as required by ndn::parseNameFromUri

ndn::HmacKey g_hmacKey(HMAC_KEY, sizeof(HMAC_KEY));

ndn::MulticastEthernetTransport g_transport0;
ndn::Face g_face0(g_transport0);
ndn_NameComponent g_comps0[8];
ndn::NameLite g_prefix0(g_comps0, 8);
ndn::PingServer g_server0(g_face0, g_prefix0);

WiFiUDP g_udp1;
ndn::UdpTransport g_transport1(g_udp1);
ndn::Face g_face1(g_transport1);
ndn_NameComponent g_comps1[8];
ndn::NameLite g_prefix1(g_comps1, 8);
ndn::PingServer g_server1(g_face1, g_prefix1);

bool
replyNoRouteNack(ndn::Face& face, const ndn::InterestLite& interest, uint64_t endpointId)
{
  ndn::NetworkNackLite nack;
  nack.setReason(ndn_NetworkNackReason_NO_ROUTE);
  face.sendNack(nack, interest, endpointId);
  Serial.print("<N ");
  Serial.println(ndn::PrintUri(interest.getName()));
  return true;
}

void
processInterest0(void*, const ndn::InterestLite& interest, uint64_t endpointId)
{
  Serial.print("0>I ");
  Serial.println(ndn::PrintUri(interest.getName()));
  g_server0.processInterest(interest, endpointId) ||
  replyNoRouteNack(g_face0, interest, endpointId);
}

void
processInterest1(void*, const ndn::InterestLite& interest, uint64_t endpointId)
{
  Serial.print("1>I ");
  Serial.println(ndn::PrintUri(interest.getName()));
  g_server1.processInterest(interest, endpointId) ||
  replyNoRouteNack(g_face1, interest, endpointId);
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
  ndn::setLogOutput(Serial);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  delay(1000);

  Serial.println(F("Please register prefixes on your router:"));
  Serial.println(F("nfdc route add /example/esp8266/ethernet [ETHER-MCAST-FACEID]"));
  Serial.print(F("nfdc face create udp4://"));
  Serial.print(WiFi.localIP());
  Serial.println(F(":6363"));
  Serial.println(F("nfdc route add /example/esp8266/udp [UDP-UNICAST-FACEID]"));
  Serial.println();
  Serial.println(F("Then you can ping:"));
  Serial.println(F("ndnping /example/esp8266/ethernet"));
  Serial.println(F("ndnping /example/esp8266/udp"));
  Serial.println();

  ndn::MulticastEthernetTransport::listNetifs(Serial);
  g_transport0.begin(ETHER_FACE_IFNAME, ETHER_FACE_IFNUM);
  g_face0.onInterest(&processInterest0, nullptr);
  g_face0.setSigningKey(g_hmacKey);
  ndn::parseNameFromUri(g_prefix0, PREFIX0);
  g_server0.onProbe(&makePayload, const_cast<void*>(reinterpret_cast<const void*>("Ethernet ndnping server")));

  g_transport1.begin(6363);
  g_face1.onInterest(&processInterest1, nullptr);
  g_face1.setSigningKey(g_hmacKey);
  ndn::parseNameFromUri(g_prefix1, PREFIX1);
  g_server1.onProbe(&makePayload, const_cast<void*>(reinterpret_cast<const void*>("UDP ndnping server")));
}

void
loop()
{
  g_face0.loop();
  g_face1.loop();
  delay(10);
}