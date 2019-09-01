#include <esp8266ndn.h>

// Heltec LoRa library
// https://github.com/HelTecAutomation/Heltec_ESP32
#include <heltec.h>

#define PIN_LED 25
#define PIN_BTN 0

ndn::DigestKey g_key;
ndn::LoraTransport<LoRaClass> g_transport(Heltec.LoRa);
ndn::Face g_face(g_transport);

char SPREFIX[] = "/example/esp32/lora/ping";
ndn::NameWCB<4> g_sPrefix;
ndn::PingServer g_server(g_face, g_sPrefix);
int g_sCount = 0;

char CPREFIX[] = "/example/esp32/lora/ping";
ndn::InterestWCB<5, 0> g_cInterest;
ndn::PingClient g_client(g_face, g_cInterest, {3000, 500}, 2000);
int g_cResponse = 0, g_cLoss = 0;
bool g_enableClient = false;

void
LOG(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  char s[32];
  vsnprintf(s, sizeof(s), fmt, args);
  va_end(args);

  Serial.println(s);

  Heltec.display->clear();
  Heltec.display->drawString(0, 40, s);
  snprintf(s, sizeof(s), "t=%d", millis());
  Heltec.display->drawString(0, 0, s);
  snprintf(s, sizeof(s), "s=%d c=%d/%d", g_sCount, g_cResponse, g_cLoss);
  Heltec.display->drawString(0, 20, s);
  Heltec.display->display();
}

void
serverProbe(void* arg, const ndn::InterestLite& interest, uint8_t* payloadBuf, size_t* payloadSize)
{
  ++g_sCount;
  LOG("S RSSI=%d SNR=%0.2f", LoRa.packetRssi(), LoRa.packetSnr());
}

void
clientEvent(void* arg, ndn::PingClient::Event evt, uint64_t seq)
{
  using Event = ndn::PingClient::Event;
  switch (evt) {
  case Event::PROBE:
    digitalWrite(PIN_LED, HIGH);
    break;
  case Event::RESPONSE:
    ++g_cResponse;
    LOG("C RSSI=%d SNR=%0.2f", LoRa.packetRssi(), LoRa.packetSnr());
    digitalWrite(PIN_LED, LOW);
    break;
  default:
    ++g_cLoss;
    LOG("C loss");
    digitalWrite(PIN_LED, LOW);
    break;
  }
}

void
setup()
{
  Heltec.begin(true, true, true, 915E6, false);
  Heltec.LoRa.setSpreadingFactor(10);
  Heltec.LoRa.setSignalBandwidth(250E3);
  Heltec.LoRa.setCodingRate4(8);
  Heltec.LoRa.setPreambleLength(16);
  Heltec.LoRa.setSyncWord(0x12);
  Heltec.LoRa.enableCrc();

  ndn::setLogOutput(Serial);
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_LED, OUTPUT);

  // LoRa MTU is 255, no need for the default 1500-octet buffer
  {
    ndn::PacketBuffer::Options opt;
    opt.maxSize = 255;
    opt.maxNameComps = 8;
    opt.maxKeyNameComps = 8;
    g_face.addReceiveBuffers(1, opt);
  }

  ndn::parseNameFromUri(g_sPrefix, SPREFIX);
  ndn::parseNameFromUri(g_cInterest.getName(), CPREFIX);
  g_face.enableTracing(Serial);
  g_face.setSigningKey(g_key);
  g_server.onProbe(&serverProbe, nullptr);
  g_client.onEvent(&clientEvent, nullptr);

  LOG("Press PRG to enable client");
}

void
loop()
{
  g_face.loop();

  if (digitalRead(PIN_BTN) == LOW) {
    g_enableClient = !g_enableClient;
    if (g_enableClient) {
      LOG("Enabling client");
    }
    else {
      LOG("Disabling client");
    }
    while (digitalRead(PIN_BTN) == LOW) // wait for key release
      ;
  }

  if (g_enableClient) {
    g_client.loop();
  }
  else {
    digitalWrite(PIN_LED, HIGH);
  }
}
