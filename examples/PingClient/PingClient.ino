#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";
const char* NDN_ROUTER_HOST = "hobo.cs.arizona.edu";
const uint16_t NDN_ROUTER_PORT = 6363;

const int LED0 = 15;
const int LED1 = 12;
const int LED2 = 13;
char PREFIX0[] = "/ndn/edu/arizona/ping"; // must be mutable
char PREFIX1[] = "/ndn/edu/ucla/ping";
char PREFIX2[] = "/ndn/edu/memphis/ping";

WiFiUDP g_udp;
ndn::UnicastUdpTransport g_transport(g_udp);
ndn::Face g_face(g_transport);

ndn_NameComponent g_comps0[8];
ndn::InterestLite g_interest0(g_comps0, 8, nullptr, 0, nullptr, 0);
ndn::PingClient g_client0(g_face, g_interest0, 17849);
ndn_NameComponent g_comps1[8];
ndn::InterestLite g_interest1(g_comps1, 8, nullptr, 0, nullptr, 0);
ndn::PingClient g_client1(g_face, g_interest1, 17345);
ndn_NameComponent g_comps2[8];
ndn::InterestLite g_interest2(g_comps2, 8, nullptr, 0, nullptr, 0);
ndn::PingClient g_client2(g_face, g_interest2, 17681);

void
processData(void*, const ndn::DataLite& data, uint64_t)
{
  g_client0.processData(data) ||
  g_client1.processData(data) ||
  g_client2.processData(data);
}

void
processNack(void*, const ndn::NetworkNackLite& nack, const ndn::InterestLite& interest, uint64_t)
{
  g_client0.processNack(nack, interest) ||
  g_client1.processNack(nack, interest) ||
  g_client2.processNack(nack, interest);
}

void
ndnpingEvent(void* arg, ndn::PingClient::Event evt, uint64_t seq)
{
  int led = reinterpret_cast<int>(arg);
  switch (evt) {
    case ndn::PingClient::Event::PROBE:
      digitalWrite(led, HIGH);
      break;
#ifdef ESP8266
    case ndn::PingClient::Event::NACK:
      analogWrite(led, PWMRANGE / 8);
      break;
#endif
    default:
      digitalWrite(led, LOW);
      break;
  }
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

  IPAddress routerIp;
  if (!WiFi.hostByName(NDN_ROUTER_HOST, routerIp)) {
    Serial.println("cannot resolve router IP");
    ESP.restart();
  }
  g_transport.begin(routerIp, NDN_ROUTER_PORT, 6363);
  g_face.onData(&processData, nullptr);
  g_face.onNack(&processNack, nullptr);

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  ndn::parseNameFromUri(g_interest0.getName(), PREFIX0);
  ndn::parseNameFromUri(g_interest1.getName(), PREFIX1);
  ndn::parseNameFromUri(g_interest2.getName(), PREFIX2);

  g_client0.onEvent(&ndnpingEvent, reinterpret_cast<void*>(LED0));
  g_client1.onEvent(&ndnpingEvent, reinterpret_cast<void*>(LED1));
  g_client2.onEvent(&ndnpingEvent, reinterpret_cast<void*>(LED2));
}

void
loop()
{
  g_face.loop();
  g_client0.loop();
  g_client1.loop();
  g_client2.loop();
  delay(10);
}
