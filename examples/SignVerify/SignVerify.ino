#include <esp8266ndn.h>

#define USE_KEY_HMAC 1
#define USE_KEY_EC   2
#define USE_KEY USE_KEY_HMAC

#if USE_KEY == USE_KEY_HMAC
const uint8_t HMACKEY[] {
  0xF3, 0x51, 0x33, 0x55, 0xF9, 0x35, 0xF5, 0x04, 0x27, 0xFA, 0x50, 0x5C, 0x96, 0x95, 0xB3, 0xAF,
  0x8B, 0x43, 0x31, 0xD5, 0xF7, 0x8E, 0x6A, 0xFF, 0x47, 0xA4, 0x96, 0x0E, 0xC5, 0x35, 0x88, 0xDA,
  0x6A, 0x23, 0x45, 0xC3, 0x35, 0x03, 0x30, 0xB3, 0x6D, 0x2A, 0x99, 0x52, 0xE3, 0xF8, 0xA3, 0xB7,
};
ndn::HmacKey g_pvtkey(HMACKEY, sizeof(HMACKEY));
ndn::HmacKey& g_pubkey = g_pvtkey;
#endif

#if USE_KEY == USE_KEY_EC
const uint8_t PVTKEY[] {
  0x6E, 0xF2, 0x17, 0xBD, 0x7C, 0x29, 0x98, 0x11, 0x57, 0xF5, 0x85, 0x34, 0x9E, 0x68, 0xF6, 0x21,
  0x28, 0x85, 0x07, 0xDC, 0x71, 0x3C, 0x2C, 0xA2, 0x2E, 0x5E, 0x0E, 0xB5, 0xAA, 0xBB, 0x3F, 0xE2,
};
const uint8_t PUBKEY[] {
  0xE6, 0x08, 0x77, 0xFD, 0xFD, 0x42, 0xC1, 0x89, 0xBF, 0x36, 0xBE, 0x2F, 0x4F, 0xD1, 0xCE, 0x03,
  0xC6, 0x18, 0xC8, 0xBE, 0x75, 0x17, 0xA4, 0x09, 0x86, 0x50, 0xD6, 0xDB, 0xAB, 0x0E, 0xAC, 0x14,
  0x12, 0x98, 0x16, 0xB6, 0x29, 0x30, 0xD5, 0x8D, 0x29, 0xA3, 0xE9, 0x94, 0xF1, 0x68, 0x35, 0x6E,
  0xC8, 0xE6, 0xEA, 0xE3, 0x7B, 0x45, 0x36, 0xB1, 0x6D, 0xA1, 0xA7, 0x19, 0x38, 0xC1, 0x0F, 0x92,
};
ndn_NameComponent g_ecKeyNameComps[4];
ndn::NameLite g_ecKeyName(g_ecKeyNameComps, 4);
ndn::EcPrivateKey g_pvtkey(PVTKEY, g_ecKeyName);
ndn::EcPublicKey g_pubkey(PUBKEY);
#endif

ndn::LoopbackTransport g_transport1, g_transport2;
ndn::Face g_face1(g_transport1), g_face2(g_transport2);

// process Interest on face1
void
processInterest1(void*, const ndn::InterestLite& interest, uint64_t)
{
  Serial << F("1-Interest ") << ndn::PrintUri(interest.getName()) << endl;

  ndn_NameComponent nameComps[4];
  ndn_NameComponent keyNameComps[4];
  ndn::DataLite data(nameComps, 4, keyNameComps, 4);
  data.getName().set(interest.getName());

  Serial << F("1-Data ") << ndn::PrintUri(data.getName()) << endl;
  g_face1.sendData(data);
}

// send Interest on face2
void
sendInterest2()
{
  ndn_NameComponent nameComps[4];
  ndn::InterestLite interest(nameComps, 4, nullptr, 0, nullptr, 0);

  ndn::NameLite& name = interest.getName();
  name.append("hello");
  uint8_t timestampBuf[9];
  name.appendTimestamp(millis(), timestampBuf, 9);

  Serial << F("2-Interest ") << ndn::PrintUri(interest.getName()) << endl;
  g_face2.sendInterest(interest);
}

// process Data on face2
void
processData2(void*, const ndn::DataLite& data, uint64_t)
{
  bool res = g_face2.verifyData(g_pubkey);
  Serial << F("2-Data ") << ndn::PrintUri(data.getName()) << F(" verified=") << static_cast<int>(res) << endl;

  sendInterest2();
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();

  g_transport1.begin(g_transport2);

#if USE_KEY == USE_KEY_EC
  g_ecKeyName.append("my-key");
#endif

  g_face1.setSigningKey(g_pvtkey);
  g_face1.onInterest(&processInterest1, nullptr);
  g_face2.onData(&processData2, nullptr);

  sendInterest2();
}

void
loop()
{
  g_face1.loop();
  g_face2.loop();
}