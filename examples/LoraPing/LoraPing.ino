#include <esp8266ndn.h>

// Heltec LoRa library
// https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/tree/master/esp32/libraries/LoRa
#include <LoRa.h>

#define PIN_LED 25
#define PIN_BTN 0

ndn::DigestKey g_key;
ndn::LoraTransport<LoRaClass> g_transport(LoRa);
ndn::Face g_face(g_transport);

char SPREFIX[] = "/example/esp32/lora/ping";
ndn::NameWCB<4> g_sPrefix;
ndn::PingServer g_server(g_face, g_sPrefix);

char CPREFIX[] = "/example/esp32/lora/ping";
ndn::InterestWCB<5, 0> g_cInterest;
ndn::PingClient g_client(g_face, g_cInterest, {3000, 500});
bool g_enableClient = true;

void
serverProbe(void* arg, const ndn::InterestLite& interest, uint8_t* payloadBuf, size_t* payloadSize)
{
  Serial.printf("S RSSI=%d SNR=%f\n", LoRa.packetRssi(), LoRa.packetSnr());
}

void
clientEvent(void* arg, ndn::PingClient::Event evt, uint64_t seq)
{
  digitalWrite(PIN_LED, evt == ndn::PingClient::Event::PROBE ? HIGH : LOW);
  if (evt == ndn::PingClient::Event::RESPONSE) {
    Serial.printf("C RSSI=%d SNR=%f\n", LoRa.packetRssi(), LoRa.packetSnr());
  }
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();
  ndn::setLogOutput(Serial);
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_LED, OUTPUT);

  // initialize LoRa
  LoRa.setTxPower(12);
  LoRa.setSpreadingFactor(10);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setCodingRate4(8);
  LoRa.setPreambleLength(16);
  LoRa.setSyncWord(0x12);
  LoRa.crc();
  LoRa.begin(915E6, true);

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
}

void
loop()
{
  g_face.loop();

  if (digitalRead(PIN_BTN) == LOW) {
    g_enableClient = !g_enableClient;
    Serial.println(g_enableClient ? "Enabling client" : "Disabling client");
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