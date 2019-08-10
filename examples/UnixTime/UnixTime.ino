#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

#ifdef ESP32
// enable comparison with ESP32 SNTP client
#define COMPARE_NTP
#endif

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

ndn::EthernetTransport g_transport;
ndn::Face g_face(g_transport);

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
    Serial.println(WiFi.status());
    delay(500);
  }
  delay(1000);

  bool ok = g_transport.begin();
  if (!ok) {
    Serial.println(F("Ethernet transport initialization failed"));
    ESP.restart();
  }

  ndn::UnixTime.begin(g_face, 5000);

#ifdef COMPARE_NTP
  configTime(0, 0, "time.nist.gov");
#endif
}

void
loop()
{
  g_face.loop();
  ndn::UnixTime.loop();

  auto now = ndn::UnixTime.now();
#ifdef COMPARE_NTP
  struct timeval tv = {0};
  int ntpOk = gettimeofday(&tv, nullptr);
#endif

  if (ndn::UnixTime.isAvailable()) {
    Serial.print("UnixTime is ");
    Serial.print(ndn::UnixTime.toRfc3399DateTime(now));
#ifdef COMPARE_NTP
    if (ntpOk == 0) {
      uint64_t ntpNow = static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
      if (now > ntpNow) {
        Serial.print("  =NTP+");
        Serial.print(static_cast<int>(now - ntpNow));
        Serial.print("us");
      }
      else {
        Serial.print("  =NTP-");
        Serial.print(static_cast<int>(ntpNow - now));
        Serial.print("us");
      }
    }
#endif
    Serial.println();
  }
  else {
    Serial.println("UnixTime is unavailable");
  }

  delay(100);
}
