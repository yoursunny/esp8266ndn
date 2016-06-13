#include <esp8266ndn.h>
#include <Streaming.h>

void demo(char* uri)
{
  ndn_NameComponent comps[4];
  ndn::NameLite name(comps, 4);

  Serial << "Input URI is " << uri << endl;
  if (ndn::parseNameFromUri(name, uri)) {
    Serial << "Name has " << name.size() << " components." << endl;
    for (int i = 0; i < name.size(); ++i) {
      Serial << "Component " << i << " has " << name.get(i).getValue().size() << " octets." << endl;
    }
    Serial << "Printed URI is " << ndn::PrintUri(name) << endl;
  }
  else {
    Serial << "parseNameFromUri fails." << endl;
  }
  Serial << endl;
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  char uri0[32] = "ndn:/";
  char uri1[32] = "/";
  char uri2[32] = "/G";
  char uri3[32] = "/H/I";
  char uri4[32] = "/J/...";
  char uri5[32] = "/.../..../.....";
  char uri6[32] = "/%00GH%ab%cD%EF";

  demo(uri0);
  demo(uri1);
  demo(uri2);
  demo(uri3);
  demo(uri4);
  demo(uri5);
  demo(uri6);
}

void loop()
{
}