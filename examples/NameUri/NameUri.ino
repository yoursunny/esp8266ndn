#include <esp8266ndn.h>

void
demo(char* uri)
{
  ndn_NameComponent comps[4];
  ndn::NameLite name(comps, 4);

  Serial.print("Input URI is ");
  Serial.println(uri);

  if (ndn::parseNameFromUri(name, uri)) {
    Serial.print("Name has ");
    Serial.print(name.size(), DEC);
    Serial.println(" components.");

    for (int i = 0; i < name.size(); ++i) {
      Serial.print("Component ");
      Serial.print(i, DEC);
      Serial.print(" has ");
      Serial.print(name.get(i).getValue().size(), DEC);
      Serial.println(" octets.");
    }

    Serial.print("Printed URI is ");
    Serial.println(ndn::PrintUri(name));
  }
  else {
    Serial.println("parseNameFromUri fails.");
  }
  Serial.println();
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();

  char uri0[] = "ndn:/";
  char uri1[] = "/";
  char uri2[] = "/G";
  char uri3[] = "/H/I";
  char uri4[] = "/J/...";
  char uri5[] = "/.../..../.....";
  char uri6[] = "/%00GH%ab%cD%EF";

  demo(uri0);
  demo(uri1);
  demo(uri2);
  demo(uri3);
  demo(uri4);
  demo(uri5);
  demo(uri6);
}

void
loop()
{
}
