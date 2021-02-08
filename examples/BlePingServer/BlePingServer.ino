// extras/BLE/BlePingClient.py is a client that can connect to this ndnping server.

#include <esp8266ndn.h>

ndnph::StaticRegion<1024> region;

esp8266ndn::BleServerTransport transport;
ndnph::Face face(transport);

ndnph::StaticRegion<2048> fragRegion;
ndnph::lp::Fragmenter fragmenter(fragRegion, transport.getMtu());
ndnph::lp::Reassembler reassembler(fragRegion);

const char* PREFIX = "/example/esp8266/ble/ping";
ndnph::PingServer server(ndnph::Name::parse(region, PREFIX), face);

void
setup()
{
  Serial.begin(115200);
  Serial.println();
  esp8266ndn::setLogOutput(Serial);

  bool ok = transport.begin("esp8266ndn");
  if (!ok) {
    Serial.println(F("BLE transport initialization failed"));
    return;
  }

  Serial.println(transport.getAddr());
  Serial.print("mtu=");
  Serial.println(transport.getMtu());

  face.setFragmenter(fragmenter);
  face.setReassembler(reassembler);
}

void
loop()
{
  face.loop();
  delay(1);
}
