#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

const char* PREFIX0 = "/ndn/edu/arizona/ping";
const char* PREFIX1 = "/ndn/edu/ucla/ping";
const char* PREFIX2 = "/ndn/edu/memphis/ping";

esp8266ndn::UdpTransport transport;
ndnph::Face face(transport);

ndnph::StaticRegion<1024> region;
ndnph::PingClient client0(ndnph::Name::parse(region, PREFIX0), face);
ndnph::PingClient client1(ndnph::Name::parse(region, PREFIX1), face);
ndnph::PingClient client2(ndnph::Name::parse(region, PREFIX2), face);

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

  IPAddress routerIp = esp8266ndn::queryFchService();
  if (routerIp == INADDR_NONE) {
    ESP.restart();
  }
  transport.beginTunnel(routerIp);
}

void
printCounters(const char* prefix, const ndnph::PingClient& client)
{
  auto cnt = client.readCounters();
  Serial.printf("%8dI %8dD %3.3f%% %s\n", static_cast<int>(cnt.nTxInterests),
                static_cast<int>(cnt.nRxData), 100.0 * cnt.nRxData / cnt.nTxInterests, prefix);
}

void
loop()
{
  face.loop();
  delay(1);

  static uint16_t i = 0;
  if (++i % 1024 == 0) {
    printCounters(PREFIX0, client0);
    printCounters(PREFIX1, client1);
    printCounters(PREFIX2, client2);
  }
}
