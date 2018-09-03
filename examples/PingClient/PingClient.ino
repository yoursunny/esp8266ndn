#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

const int LED0 = 15;
const int LED1 = 12;
const int LED2 = 13;
char PREFIX0[] = "/ndn/edu/arizona/ping"; // must be mutable
char PREFIX1[] = "/ndn/edu/ucla/ping";
char PREFIX2[] = "/ndn/edu/memphis/ping";

ndn::UdpTransport g_transport;
ndn::Face g_face(g_transport);

ndn::InterestWCB<8, 0> g_interest0;
ndn::PingClient g_client0(g_face, g_interest0, 17849);
ndn::InterestWCB<8, 0> g_interest1;
ndn::PingClient g_client1(g_face, g_interest1, 17345);
ndn::InterestWCB<8, 0> g_interest2;
ndn::PingClient g_client2(g_face, g_interest2, 17681);

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

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  delay(1000);

  IPAddress routerIp = ndn::queryFchService();
  if (routerIp == INADDR_NONE) {
    ESP.restart();
  }
  g_transport.beginTunnel(routerIp);
  g_face.enableTracing(Serial);

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
