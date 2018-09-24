// g++ -std=c++14 -o ec-demo ec-demo.cpp $(pkg-config --cflags --libs libndn-cxx)

#include <cinttypes>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include <ndn-cxx/security/transform.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
#include <ndn-cxx/security/v2/key-chain.hpp>
#include <ndn-cxx/util/indented-stream.hpp>

bool readKeyBits(FILE* input, uint8_t* output, int len) {
  for (int i = 0; i < len; ++i) {
    if (fscanf(input, " %2" SCNx8 "%*[:]", output + i) != 1) {
      return false;
    }
  }
  return true;
}

void printHexArray(std::ostream& os, const std::string& var, const uint8_t* buf, int len, bool isProgmem = false) {
  os << "const uint8_t " << var << "[] " << (isProgmem ? "PROGMEM " : "") << " {\n";

  ndn::util::IndentedStream osi(os, "  ");
  osi << std::hex << std::uppercase << std::setfill('0');
  for (int i = 0; i < len; ++i) {
    osi << "0x" << std::setw(2) << static_cast<int>(buf[i]) << ',' << (i % 16 == 15 ? '\n' : ' ');
  }
  osi.flush();

  os << "};\n";
}

void printHexLine(std::ostream& os, const std::string& var, const uint8_t* buf, int len) {
  os << var << "= ";

  using namespace ndn::security::transform;
  StepSource ss;
  ss >> hexEncode(true) >> streamSink(os);
  ss.write(buf, len);
  ss.end();

  os << '\n';
}

int main() {
  // generate key
  ndn::KeyChain keyChain("pib-memory:", "tpm-memory:");
  auto identity = keyChain.createIdentity("/ec-demo", ndn::EcKeyParams(256));

  // export key
  const std::string password = "0000";
  auto safeBag = keyChain.exportSafeBag(identity.getDefaultKey().getDefaultCertificate(), password.data(), password.size());
  auto keyBag = safeBag->getEncryptedKeyBag();
  std::string pkcs8File = std::tmpnam(nullptr);
  std::ofstream pkcs8Stream(pkcs8File, std::ios_base::binary);
  pkcs8Stream.write(reinterpret_cast<const char*>(keyBag.data()), keyBag.size());
  pkcs8Stream.close();

  // extract key bits with OpenSSL
  FILE* openssl = popen(("openssl pkcs8 -inform DER -in " + pkcs8File + " -passin pass:" + password + " | openssl ec -noout -text 2>/dev/null; rm -f " + pkcs8File).data(), "r");
  uint8_t pvtkeyBits[32], pubkeyBits[65];
  char posTest[2];
  if (!(fscanf(openssl, "Private-Key: (256 bit) priv: ") == 0 &&
        readKeyBits(openssl, pvtkeyBits, sizeof(pvtkeyBits)) &&
        fscanf(openssl, " pu%1[b]: ", posTest) == 1 &&
        readKeyBits(openssl, pubkeyBits, sizeof(pubkeyBits)))) {
    return 3;
  }

  // print key as array for use in code
  printHexArray(std::cout, "PVTKEY", pvtkeyBits, sizeof(pvtkeyBits), true);
  printHexArray(std::cout, "PUBKEY", pubkeyBits, sizeof(pubkeyBits), true);

  // print key as line for use in demo program
  printHexLine(std::cout, "pvtkey", pvtkeyBits, sizeof(pvtkeyBits));
  printHexLine(std::cout, "pubkey", pubkeyBits, sizeof(pubkeyBits));

  // print input
  std::string input = "yoursunny.com esp8266ndn ECDSA signing demo";
  printHexLine(std::cout, "input", reinterpret_cast<const uint8_t*>(input.data()), input.size());

  auto pubkey = identity.getDefaultKey().getPublicKey();
  while (true) {
    // print ndn-cxx signature
    auto sig = keyChain.sign(reinterpret_cast<const uint8_t*>(input.data()), input.size());
    sig.encode();
    printHexLine(std::cout, "pc-sig", sig.value(), sig.value_size());

    // input and verify esp8266ndn signature
    std::cout << "esp-sig= ";
    char espSigLine[150];
    std::cin.getline(espSigLine, sizeof(espSigLine));
    std::string espSig;
    {
      std::istringstream is(espSigLine);
      std::ostringstream os;
      namespace t = ndn::security::transform;
      try {
        t::streamSource(is) >> t::hexDecode() >> t::streamSink(os);
      } catch (const t::Error&) {}
      espSig = os.str();
    }

    bool res = ndn::security::verifySignature(reinterpret_cast<const uint8_t*>(input.data()), input.size(), reinterpret_cast<const uint8_t*>(espSig.data()), espSig.size(), pubkey.data(), pubkey.size());
    std::cout << (res ? "res=OK\n" : "res=BAD\n");
  }
}