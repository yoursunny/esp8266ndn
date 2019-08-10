#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include <esp8266ndn.h>

// ESP32 only: uncomment to compare with NTP.
// See (ndnTime - ntpTime) in micros via Arduino IDE serial plotter.
// Remember that lwip SNTP client can have error, too.
// #define COMPARE_NTP

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

ndn::EthernetTransport g_transport;
ndn::Face g_face(g_transport);

void
setup()
{
  Serial.begin(115200);
  Serial.println();
#ifndef COMPARE_NTP
  ndn::setLogOutput(Serial);
#endif

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  delay(1000);

  bool ok = g_transport.begin();
  if (!ok) {
    Serial.println(F("Ethernet transport initialization failed"));
    ESP.restart();
  }

#ifdef COMPARE_NTP
  configTime(0, 0, "time.nist.gov");
  ndn::UnixTime.disableIntegration();
#endif

  ndn::UnixTime.begin(g_face, 5000);
}

void
loop()
{
  g_face.loop();
  ndn::UnixTime.loop();

#if defined(COMPARE_NTP) // NTP error plot
  auto now = ndn::UnixTime.now();
  struct timeval tv = {0};
  gettimeofday(&tv, nullptr);
  uint64_t ntpNow = static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
  Serial.println(static_cast<int>(now - ntpNow));
#else

# ifdef ESP32 // ESP32 only: system clock integration
  struct tm t = {0};
  if (getLocalTime(&t, 10)) {
    Serial.printf("%04d-%02d-%02dT%02d:%02d:%02d        (SYS)\n",
                  1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday,
                  t.tm_hour, t.tm_min, t.tm_sec);
  }
# endif

  if (ndn::UnixTime.isAvailable()) {
    auto now = ndn::UnixTime.now();
    Serial.println(ndn::UnixTime.toRfc3399DateTime(now));
  }
  else {
    Serial.println("unavailable");
  }
#endif

  delay(100);
}
