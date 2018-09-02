#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

ndn::DigestKey g_key;

ndn::MulticastEthernetTransport g_transport0;
ndn::Face g_face0(g_transport0);
char PREFIX0[] = "/example/esp8266/ether/ping";
ndn::NameWCB<8> g_prefix0;
ndn::PingServer g_server0(g_face0, g_prefix0);

ndn::UdpTransport g_transport1;
ndn::Face g_face1(g_transport1);
char PREFIX1[] = "/example/esp8266/udp/ping";
ndn::NameWCB<8> g_prefix1;
ndn::PingServer g_server1(g_face1, g_prefix1);

ndn::UdpTransport g_transport2;
ndn::Face g_face2(g_transport2);
char PREFIX2[] = "/example/esp8266/udpm/ping";
ndn::NameWCB<8> g_prefix2;
ndn::PingServer g_server2(g_face2, g_prefix2);

void
makePayload(void* arg, const ndn::InterestLite& interest, uint8_t* payloadBuf, size_t* payloadSize)
{
  auto text = reinterpret_cast<const char*>(arg);
  size_t len = strlen(text);
  memcpy(payloadBuf, text, len);
  *payloadSize = len;
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();
  ndn::setLogOutput(Serial);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  delay(1000);

  ndn::MulticastEthernetTransport::listNetifs(Serial);
  bool ok = g_transport0.begin(); // select any STA netif
  if (!ok) {
    Serial.println("Ethernet transport initialization failed");
    ESP.restart();
  }
  g_face0.setSigningKey(g_key);
  ndn::parseNameFromUri(g_prefix0, PREFIX0);
  g_server0.onProbe(&makePayload, const_cast<void*>(reinterpret_cast<const void*>("Ethernet ndnping server")));

  ok = g_transport1.beginListen();
  if (!ok) {
    Serial.println("UDP unicast transport initialization failed");
    ESP.restart();
  }
  g_face1.setSigningKey(g_key);
  ndn::parseNameFromUri(g_prefix1, PREFIX1);
  g_server1.onProbe(&makePayload, const_cast<void*>(reinterpret_cast<const void*>("UDP unicast ndnping server")));

  ok = g_transport2.beginMulticast(WiFi.localIP());
  if (!ok) {
    Serial.println("UDP multicast transport initialization failed");
    ESP.restart();
  }
  g_face2.setSigningKey(g_key);
  ndn::parseNameFromUri(g_prefix2, PREFIX2);
  g_server2.enableEndpointIdZero();
  g_server2.onProbe(&makePayload, const_cast<void*>(reinterpret_cast<const void*>("UDP multicast ndnping server")));

  Serial.println(F("Please register prefixes on your router:"));
  Serial.println(F("nfdc route add /example/esp8266/ether [ETHER-MCAST-FACEID]"));
  Serial.print(F("nfdc face create udp4://"));
  Serial.print(WiFi.localIP());
  Serial.println(F(":6363"));
  Serial.println(F("nfdc route add /example/esp8266/udp [UDP-UNICAST-FACEID]"));
  Serial.println(F("nfdc route add /example/esp8266/udpm [UDP-MCAST-FACEID]"));
  Serial.println();
  Serial.println(F("Then you can ping:"));
  Serial.println(F("ndnping /example/esp8266/ether"));
  Serial.println(F("ndnping /example/esp8266/udp"));
  Serial.println(F("ndnping /example/esp8266/udpm"));
  Serial.println();
}

void
loop()
{
  g_face0.loop();
  g_face1.loop();
  g_face2.loop();
  delay(10);
}