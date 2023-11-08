#if defined(ARDUINO_ARCH_ESP8266)
#include <AddrList.h>
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

ndnph::StaticRegion<1024> region;

esp8266ndn::EthernetTransport transport0;
ndnph::Face face0(transport0);
const char* PREFIX0 = "/example/esp8266/ether/ping";
ndnph::PingServer server0(ndnph::Name::parse(region, PREFIX0), face0);

std::array<uint8_t, esp8266ndn::UdpTransport::DefaultMtu> udpBuffer;
esp8266ndn::UdpTransport transport1(udpBuffer);
ndnph::Face face1(transport1);
const char* PREFIX1 = "/example/esp8266/udp/ping";
ndnph::PingServer server1(ndnph::Name::parse(region, PREFIX1), face1);

esp8266ndn::UdpTransport transport2(udpBuffer);
ndnph::transport::ForceEndpointId transport2w(transport2);
ndnph::Face face2(transport2w);
const char* PREFIX2 = "/example/esp8266/udpm/ping";
ndnph::PingServer server2(ndnph::Name::parse(region, PREFIX2), face2);

void
setup() {
  Serial.begin(115200);
  Serial.println();
  esp8266ndn::setLogOutput(Serial);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(F("WiFi connect failed"));
    ESP.restart();
  }
#if defined(ARDUINO_ARCH_ESP8266) && LWIP_IPV6
  { // wait until no new address showing up in 1000ms
    size_t nAddrPrev = 0, nAddr = 0;
    do {
      delay(1000);
      nAddrPrev = nAddr;
      nAddr = 0;
      for (auto a : addrList) {
        (void)a;
        ++nAddr;
      }
    } while (nAddrPrev == nAddr);
  }
#else
  delay(1000);
#endif

  esp8266ndn::EthernetTransport::listNetifs(Serial);
  bool ok = transport0.begin(); // select any STA netif
  if (!ok) {
    Serial.println(F("Ethernet transport initialization failed"));
    ESP.restart();
  }

  ok = transport1.beginListen();
  if (!ok) {
    Serial.println(F("UDP unicast transport initialization failed"));
    ESP.restart();
  }

  ok = transport2.beginMulticast(WiFi.localIP());
  if (!ok) {
    Serial.println(F("UDP multicast transport initialization failed"));
    ESP.restart();
  }

  Serial.println(F("Please register prefixes on your router:"));
  Serial.println(F("nfdc route add /example/esp8266/ether [ETHER-MCAST-FACEID]"));
#if defined(ARDUINO_ARCH_ESP8266)
  for (auto a : addrList) {
    if (a.isV4()) {
      Serial.print(F("nfdc face create udp4://"));
    } else {
      Serial.print(F("nfdc face create udp6://["));
    }
    Serial.print(a.addr());
    if (a.isV4()) {
      Serial.println(F(":6363"));
    } else {
      Serial.println(F("]:6363"));
    }
  }
#elif defined(ARDUINO_ARCH_ESP32)
  Serial.print(F("nfdc face create udp4://"));
  Serial.print(WiFi.localIP());
  Serial.println(F(":6363"));
#endif
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
loop() {
  face0.loop();
  face1.loop();
  face2.loop();
  delay(1);
}
