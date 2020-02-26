#include <esp8266ndn.h>

#include <Arduino.h>
#include <ArduinoUnit.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#elif defined(ARDUINO_ARCH_NRF52)
#include <bluefruit.h>
#endif

ndnph::StaticRegion<4096> region;

// Interest, Data, Interest-Data match, Data signing, implicit digest, DigestKey
test(InterestData)
{
  region.reset();

  auto data = region.create<ndnph::Data>();
  assertFalse(!data);
  data.setName(ndnph::Name::parse(region, "/A"));
  uint8_t implicitDigest[NDNPH_SHA256_LEN];
  {
    ndnph::Encoder encoder(region);
    encoder.prepend(data.sign(ndnph::DigestKey()));
    encoder.trim();

    data = region.create<ndnph::Data>();
    assertFalse(!data);
    assertTrue(ndnph::Decoder(encoder.begin(), encoder.size()).decode(data));
    assertTrue(data.computeImplicitDigest(implicitDigest));
  }

  auto interest = region.create<ndnph::Interest>();
  assertFalse(!interest);
  interest.setName(data.getName().append(
    region, { ndnph::Component(region, ndnph::TT::ImplicitSha256DigestComponent,
                               sizeof(implicitDigest), implicitDigest) }));
  assertTrue(interest.match(data));
}

// Interest signing, ECDSA
test(Ecdsa)
{
  region.reset();
  auto keyName = ndnph::Name::parse(region, "/K");

  ndnph::EcdsaPrivateKey pvt;
  ndnph::EcdsaPublicKey pub;
  assertTrue(ndnph::EcdsaPrivateKey::generate(keyName, pvt, pub));

  auto interest = region.create<ndnph::Interest>();
  assertFalse(!interest);
  interest.setName(ndnph::Name::parse(region, "/A"));
  {
    ndnph::Encoder encoder(region);
    encoder.prepend(interest.sign(pvt));
    encoder.trim();

    interest = region.create<ndnph::Interest>();
    assertFalse(!interest);
    assertTrue(ndnph::Decoder(encoder.begin(), encoder.size()).decode(interest));
  }

  assertTrue(interest.verify(pub));
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();

  // Enable radio, which is need by hardware random number generator.
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
#elif defined(ARDUINO_ARCH_NRF52)
  Bluefruit.begin();
#endif
}

void
loop()
{
  Test::run();
}
