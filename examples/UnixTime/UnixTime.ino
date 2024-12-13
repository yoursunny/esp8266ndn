#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#define CHIP ESP
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#define CHIP ESP
#elif defined(ARDUINO_ARCH_RP2040)
#include <WiFi.h>
#define CHIP rp2040
#endif
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";

esp8266ndn::UdpTransport transport;
ndnph::Face face(transport);
esp8266ndn::UnixTime unixTime(face);

void
printRfc3399DateTime(uint64_t timestamp) {
  time_t sec = timestamp / 1000000;
  int usec = static_cast<int>(timestamp % 1000000);

  struct tm t;
  if (gmtime_r(&sec, &t) == nullptr) {
    Serial.println("gmtime_r error");
  }

  Serial.printf("%04d-%02d-%02dT%02d:%02d:%02d.%06dZ\n", 1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday,
                t.tm_hour, t.tm_min, t.tm_sec, usec);
}

void
setup() {
  Serial.begin(115200);
  Serial.println();
  esp8266ndn::setLogOutput(Serial);

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
#elif defined(ARDUINO_ARCH_RP2040)
  WiFi.noLowPowerMode();
#endif
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(F("WiFi connect failed"));
    CHIP.restart();
  }
  delay(1000);

  bool ok = transport.beginMulticast(WiFi.localIP());
  if (!ok) {
    Serial.println(F("UDP multicast initialization failed"));
    CHIP.restart();
  }

  unixTime.begin(5000);
}

void
loop() {
  face.loop();

  auto now = ndnph::port::UnixTime::now();
  printRfc3399DateTime(now);
  delay(100);
}
