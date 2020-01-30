#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

// const int LED0 = 15;
// const int LED1 = 12;
// const int LED2 = 13;
const char* PREFIX0 = "/ndn/edu/arizona/ping";
const char* PREFIX1 = "/ndn/edu/ucla/ping";
const char* PREFIX2 = "/ndn/edu/memphis/ping";

esp8266ndn::UdpTransport transport;
ndnph::Face face(transport);

ndnph::StaticRegion<1024> region;
ndnph::PingClient client0(ndnph::Name::parse(region, PREFIX0), face);
ndnph::PingClient client1(ndnph::Name::parse(region, PREFIX1), face);
ndnph::PingClient client2(ndnph::Name::parse(region, PREFIX2), face);

// void
// ndnpingEvent(void* arg, ndn::PingClient::Event evt, uint64_t seq)
// {
//   int led = reinterpret_cast<int>(arg);
//   switch (evt) {
//     case ndn::PingClient::Event::PROBE:
//       digitalWrite(led, HIGH);
//       break;
// #ifdef ESP8266
//     case ndn::PingClient::Event::NACK:
//       analogWrite(led, PWMRANGE / 8);
//       break;
// #endif
//     default:
//       digitalWrite(led, LOW);
//       break;
//   }
// }

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

  // IPAddress routerIp = ndn::queryFchService();
  // if (routerIp == INADDR_NONE) {
  //   ESP.restart();
  // }
  transport.beginTunnel(IPAddress(192, 168, 5, 10));
  // face.enableTracing(Serial);

  // pinMode(LED0, OUTPUT);
  // pinMode(LED1, OUTPUT);
  // pinMode(LED2, OUTPUT);

  // g_client0.onEvent(&ndnpingEvent, reinterpret_cast<void*>(LED0));
  // g_client1.onEvent(&ndnpingEvent, reinterpret_cast<void*>(LED1));
  // g_client2.onEvent(&ndnpingEvent, reinterpret_cast<void*>(LED2));
}

void
printCounters(const char* prefix, const ndnph::PingClient& client)
{
  auto cnt = client.readCounters();
  Serial.printf("%8d %8d %2.3f%% %s\n", static_cast<int>(cnt.nTxInterests),
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
