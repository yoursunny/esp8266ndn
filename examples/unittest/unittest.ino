#include <esp8266ndn.h>

#include <Arduino.h>
#include <ArduinoUnit.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <FFat.h>
#include <WiFi.h>
#elif defined(ARDUINO_ARCH_NRF52)
#include <InternalFileSystem.h>
#include <bluefruit.h>
#endif

ndnph::StaticRegion<4096> region;
ndnph::KeyChain keyChain;
bool keyChainOpenResult = false;

// Interest, Data, Interest-Data match, Data signing, implicit digest, DigestKey
test(InterestData) {
  region.reset();

  auto data = region.create<ndnph::Data>();
  assertFalse(!data);
  data.setName(ndnph::Name::parse(region, "/A"));
  uint8_t implicitDigest[NDNPH_SHA256_LEN];
  {
    ndnph::Encoder encoder(region);
    encoder.prepend(data.sign(ndnph::DigestKey::get()));
    encoder.trim();

    data = region.create<ndnph::Data>();
    assertFalse(!data);
    assertTrue(ndnph::Decoder(encoder.begin(), encoder.size()).decode(data));
    assertTrue(data.computeImplicitDigest(implicitDigest));
  }

  auto interest = region.create<ndnph::Interest>();
  assertFalse(!interest);
  interest.setName(
    data.getName().append(region, ndnph::convention::ImplicitDigest(), implicitDigest));
  assertTrue(data.canSatisfy(interest));
}

// Interest signing, ECDSA, KeyChain
test(Ecdsa) {
  region.reset();
  auto subjectName = ndnph::Name::parse(region, "/K");

  assertTrue(keyChainOpenResult);
  ndnph::EcPrivateKey pvt0, pvt1;
  ndnph::EcPublicKey pub0, pub1;
  assertTrue(ndnph::ec::generate(region, subjectName, pvt0, pub0, keyChain, "k"));
  assertTrue(ndnph::ec::load(keyChain, "k", region, pvt1, pub1));
  assertEqual(pvt1.getName(), pvt0.getName());
  assertEqual(pub1.getName(), pub0.getName());

  auto interest = region.create<ndnph::Interest>();
  assertFalse(!interest);
  auto data = region.create<ndnph::Data>();
  assertFalse(!data);
  interest.setName(ndnph::Name::parse(region, "/I"));
  data.setName(ndnph::Name::parse(region, "/D"));
  {
    ndnph::Encoder encoderI(region);
    encoderI.prepend(interest.sign(pvt0));
    encoderI.trim();

    interest = region.create<ndnph::Interest>();
    assertFalse(!interest);
    assertTrue(ndnph::Decoder(encoderI.begin(), encoderI.size()).decode(interest));

    ndnph::Encoder encoderD(region);
    encoderD.prepend(data.sign(pvt1));
    encoderD.trim();

    data = region.create<ndnph::Data>();
    assertFalse(!data);
    assertTrue(ndnph::Decoder(encoderD.begin(), encoderD.size()).decode(data));
  }

  assertTrue(interest.verify(pub1));
  assertTrue(data.verify(pub0));
}

// HMAC-SHA256
test(Hmac) {
  // https://datatracker.ietf.org/doc/html/rfc4231#section-4.4
  std::array<uint8_t, 20> buffer;
  buffer.fill(0xAA);
  ndnph::HmacKey key;
  assertTrue(key.import(buffer.data(), 20));
  buffer.fill(0xDD);

  std::array<uint8_t, NDNPH_SHA256_LEN> sig;
  assertEqual(key.sign({ndnph::tlv::Value(buffer.data(), 20), ndnph::tlv::Value(buffer.data(), 15),
                        ndnph::tlv::Value(buffer.data(), 15)},
                       sig.data()),
              static_cast<ssize_t>(sig.size()));

  std::array<uint8_t, NDNPH_SHA256_LEN> expectedSig{
    0x77, 0x3E, 0xA9, 0x1E, 0x36, 0x80, 0x0E, 0x46, 0x85, 0x4D, 0xB8, 0xEB, 0xD0, 0x91, 0x81, 0xA7,
    0x29, 0x59, 0x09, 0x8B, 0x3E, 0xF8, 0xC1, 0x22, 0xD9, 0x63, 0x55, 0x14, 0xCE, 0xD5, 0x65, 0xFE};
  for (size_t i = 0; i < expectedSig.size(); ++i) {
    assertEqual(sig[i], expectedSig[i], i);
  }

  assertTrue(key.verify({ndnph::tlv::Value(buffer.data(), 10), ndnph::tlv::Value(buffer.data(), 20),
                         ndnph::tlv::Value(buffer.data(), 20)},
                        sig.data(), sig.size()));

  assertFalse(key.verify({ndnph::tlv::Value(buffer.data(), 17)}, sig.data(), sig.size()));

  sig[15] ^= 0x01;
  assertFalse(
    key.verify({ndnph::tlv::Value(buffer.data(), 10), ndnph::tlv::Value(buffer.data(), 20),
                ndnph::tlv::Value(buffer.data(), 20)},
               sig.data(), sig.size()));
}

void
setup() {
#if ARDUINO_USB_CDC_ON_BOOT
  while (!Serial) {
    delay(1);
  }
#endif
  Serial.begin(115200);
  Serial.println();

  // Radio is needed by hardware random number generator.
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
#elif defined(ARDUINO_ARCH_NRF52)
  Bluefruit.begin();
#endif

  // Filesystem is needed by KeyChain.
#if defined(ARDUINO_ARCH_ESP8266)
  LittleFS.begin();
  LittleFS.format();
#elif defined(ARDUINO_ARCH_ESP32)
  FFat.begin();
  FFat.format();
#elif defined(ARDUINO_ARCH_NRF52)
  InternalFS.begin();
  InternalFS.format();
#endif
  delay(1);
  keyChainOpenResult = keyChain.open("/keychain");
}

void
loop() {
  Test::run();
}
