#include <FFat.h>
#include <WiFi.h>
#include <esp8266ndn.h>

const char* WIFI_SSID = "my-ssid";
const char* WIFI_PASS = "my-pass";
const IPAddress NDN_ROUTER(192, 0, 2, 1);
const char* SUBJECT_NAME = "/org/example/user";
const char* CA_NAME = "/org/example";

ndnph::StaticRegion<65536> region;
ndnph::StaticRegion<2048> packetRegion;
ndnph::KeyChain keyChain;
esp8266ndn::UdpTransport transport;
ndnph::Face face(transport);

const char* keySlot = "ndncertdemokey";
const char* certSlot = "ndncertdemocert";
ndnph::EcPrivateKey pvt;
ndnph::EcPublicKey pub;

enum class State
{
  Idle,
  StartDiscoverProfileVersion,
  WaitDiscoverProfileVersion,
  StartRetrieveProfile,
  WaitRetrieveProfile,
  StartNdncertClient,
  WaitNdncertClient,
  Success,
  Failure,
  Final,
};
State state = State::Idle;

std::unique_ptr<ndnph::RdrMetadataConsumer> profileRdr;
ndnph::Name profileVersion;
std::unique_ptr<ndnph::SegmentConsumer> profileSegment;
ndnph::ndncert::client::CaProfile profile;
ndnph::ndncert::client::NopChallenge nop;
std::unique_ptr<ndnph::ndncert::Client> client;
std::unique_ptr<ndnph::PingServer> ping;

bool
initKey()
{
  packetRegion.reset();
  auto subjectName = ndnph::Name::parse(packetRegion, SUBJECT_NAME);
  bool ok = ndnph::ec::load(keyChain, keySlot, region, pvt, pub);
  if (ok && ndnph::certificate::toSubjectName(packetRegion, pvt.getName()) == subjectName) {
    Serial.print("[demo] found existing key: ");
    Serial.println(pvt.getName());
    return true;
  }

  ok = ndnph::ec::generate(region, subjectName, pvt, pub, keyChain, keySlot);
  if (ok) {
    Serial.print("[demo] generating key: ");
    Serial.println(pvt.getName());
    return true;
  }

  Serial.println("[demo] key generation failed");
  return false;
}

void
setup()
{
  Serial.begin(115200);
  Serial.println();
  esp8266ndn::setLogOutput(Serial);
  pinMode(0, INPUT_PULLUP);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(F("WiFi connect failed"));
    ESP.restart();
  }
  configTime(0, 0, "time.nist.gov");
  delay(2000);

  struct tm tm = {};
  bool ok = getLocalTime(&tm);
  if (!ok) {
    Serial.println("[demo] SNTP failed");
    ESP.restart();
  }

  ok = FFat.begin(true) && keyChain.open("/keychain");
  if (!ok) {
    Serial.println("[demo] open KeyChain failed");
    ESP.restart();
  }

  ok = transport.beginTunnel(NDN_ROUTER);
  if (!ok) {
    Serial.println("[demo] UDP transport initialization failed");
    ESP.restart();
  }

  ok = initKey();
  if (!ok) {
    ESP.restart();
  }

  state = State::StartDiscoverProfileVersion;
}

void
rdrCallback(void*, ndnph::Data rdrMetadata)
{
  profileVersion = ndnph::rdr::parseMetadata(rdrMetadata).clone(region);
  profileRdr.reset();
  if (profileVersion.size() == 0) {
    Serial.println("[demo] RDR failed");
    state = State::Failure;
    return;
  }

  Serial.print("[demo] RDR response: ");
  Serial.println(profileVersion);
  state = State::StartRetrieveProfile;
}

void
segmentCallback(void*, uint64_t, ndnph::Data data)
{
  if (!profile.fromData(region, data)) {
    Serial.print("[demo] bad CA profile: ");
    Serial.println(data);
    state = State::Failure;
    return;
  }

  Serial.print("[demo] retrieved CA profile: ");
  Serial.println(profile.prefix);
  state = State::StartNdncertClient;
}

void
ndncertCallback(void*, ndnph::Data cert)
{
  if (!cert) {
    Serial.println("[demo] ndncert client failed");
    state = State::Failure;
    return;
  }

  packetRegion.reset();
  if (!keyChain.certs.set(certSlot, cert, packetRegion)) {
    Serial.println("[demo] save cert failed");
    state = State::Failure;
    return;
  }

  Serial.print("[demo] ndncert client success: ");
  Serial.println(cert.getFullName(packetRegion));
  state = State::Success;
}

void
loop()
{
  face.loop();
  packetRegion.reset();

  switch (state) {
    case State::StartDiscoverProfileVersion: {
      profileRdr.reset(new ndnph::RdrMetadataConsumer(face));
      profileRdr->start(
        ndnph::Name::parse(packetRegion, CA_NAME)
          .append(region, ndnph::ndncert::getCaComponent(), ndnph::ndncert::getInfoComponent()),
        rdrCallback, nullptr);
      state = State::WaitDiscoverProfileVersion;
      break;
    }
    case State::StartRetrieveProfile: {
      profileSegment.reset(new ndnph::SegmentConsumer(face));
      profileSegment->setSegmentCallback(segmentCallback, nullptr);
      profileSegment->start(profileVersion);
      state = State::WaitRetrieveProfile;
      break;
    }
    case State::WaitRetrieveProfile: {
      if (!profileSegment->isRunning()) {
        profileSegment.reset();
        state = State::StartNdncertClient;
      }
      break;
    }
    case State::StartNdncertClient: {
      ndnph::ndncert::Client::requestCertificate(ndnph::ndncert::Client::Options{
        .face = face,
        .profile = profile,
        .challenges = { &nop },
        .pub = pub,
        .pvt = pvt,
        .cb = ndncertCallback,
        .ctx = nullptr,
      });
      state = State::WaitNdncertClient;
      break;
    }
    case State::Success:
    case State::Failure: {
      client.reset();
      Serial.println("[demo] press RESET to rerun demo");
      Serial.println("[demo] press button #0 to delete saved key and rerun demo");
      state = State::Final;
      break;
    }
    case State::Final: {
      if (digitalRead(0) == LOW) {
        keyChain.keys.del(keySlot);
        keyChain.certs.del(certSlot);
        Serial.println("[demo] deleting saved key and certificate");
        delay(100);
        ESP.restart();
      }
      break;
    }
    default:
      break;
  }
  delay(1);
}
