#ifdef ESP32
#include <esp8266ndn.h>

char PREFIX[] = "/example/esp32-ble/ping";

ndn::BleServerTransport g_transport;
ndn::Face g_face(g_transport);
ndn::DigestKey g_pvtkey;

ndn_NameComponent g_comps[4];
ndn::NameLite g_prefix(g_comps, 4);
ndn::PingServer g_server(g_face, g_prefix);

bool
replyNoRouteNack(const ndn::InterestLite& interest)
{
  ndn::NetworkNackLite nack;
  nack.setReason(ndn_NetworkNackReason_NO_ROUTE);
  g_face.sendNack(nack, interest);
  Serial.print("<N ");
  Serial.println(ndn::PrintUri(interest.getName()));
  return true;
}

void
processInterest(void*, const ndn::InterestLite& interest, uint64_t)
{
  Serial.print(">I ");
  Serial.println(ndn::PrintUri(interest.getName()));
  g_server.processInterest(interest) ||
  replyNoRouteNack(interest);
}

void
makePayload(void* arg, const ndn::InterestLite& interest, uint8_t* payloadBuf, size_t* payloadSize)
{
  auto text = reinterpret_cast<const char*>(arg);
  size_t len = strlen(text);
  memcpy(payloadBuf, text, len);
  *payloadSize = len;
  Serial.print("<D ");
  Serial.println(ndn::PrintUri(interest.getName()));
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();
  ndn::setLogOutput(Serial);

  g_transport.begin("ESP32-BLE-NDN");
  g_face.onInterest(&processInterest, nullptr);
  g_face.setSigningKey(g_pvtkey);

  ndn::parseNameFromUri(g_prefix, PREFIX);

  g_server.onProbe(&makePayload, const_cast<void*>(reinterpret_cast<const void*>("PingServer")));
}

void
loop()
{
  g_face.loop();
  delay(100);
}

#else // ESP32

void
setup()
{
}

void
loop()
{
}

#endif // ESP32