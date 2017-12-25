#include <esp8266ndn.h>

const uint8_t THE_INPUT[] {
  0x79, 0x6F, 0x75, 0x72, 0x73, 0x75, 0x6E, 0x6E, 0x79, 0x2E, 0x63, 0x6F, 0x6D, 0x20, 0x65, 0x73,
  0x70, 0x38, 0x32, 0x36, 0x36, 0x6E, 0x64, 0x6E, 0x20, 0x45, 0x43, 0x44, 0x53, 0x41, 0x20, 0x73,
  0x69, 0x67, 0x6E, 0x69, 0x6E, 0x67, 0x20, 0x64, 0x65, 0x6D, 0x6F,
};
uint8_t pvtkey[] {
  0xFB, 0xF9, 0xA3, 0xAB, 0xB0, 0x96, 0xD7, 0x56, 0xFF, 0x65, 0xD7, 0xE2, 0x7D, 0x71, 0x6B, 0x89,
  0x21, 0xB1, 0xC6, 0x86, 0xC0, 0x4E, 0xAD, 0x9C, 0xE0, 0x1C, 0xCD, 0x05, 0x5A, 0x7B, 0xD0, 0x3D,
};
uint8_t pubkey[] {
  0xCF, 0xD2, 0x75, 0x43, 0x38, 0x02, 0xD2, 0x5B, 0x28, 0x9C, 0xCE, 0x3D, 0x19, 0x11, 0x9D, 0x84,
  0x2E, 0xDE, 0x21, 0x16, 0x19, 0xB8, 0x24, 0xD4, 0x8F, 0x4C, 0x13, 0xEB, 0x6D, 0x06, 0xD1, 0x97,
  0x35, 0x0C, 0x14, 0x0E, 0x03, 0xAF, 0x75, 0x13, 0x67, 0x6D, 0xD0, 0x54, 0x3C, 0xF2, 0x8A, 0x5E,
  0xBE, 0x47, 0xBB, 0xE0, 0x77, 0x6C, 0x84, 0x8F, 0xF1, 0xE6, 0xBB, 0x2A, 0x98, 0x02, 0xB0, 0xB7,
};

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10000);
  delay(2000);
  Serial.println();
  Serial.println();
}

void printHex(const uint8_t* buf, size_t len) {
  for (int i = 0; i < len; ++i) {
    uint8_t ch = buf[i];
    if (ch < 0x10) {
      Serial.print('0');
    }
    Serial.print(ch, 16);
  }
  Serial.println();
}

const char* promptLine(const __FlashStringHelper* prompt) {
  static char line[150];
  size_t len;
  do {
    Serial.println();
    Serial.print(prompt);
    len = Serial.readBytesUntil('\n', line, sizeof(line) - 1);
  } while (len == 0);
  line[len] = '\0';
  Serial.print(line);
  Serial.println();
  return line;
}

int parseHex(int ch) {
  if ('0' <= ch && ch <= '9') {
    return ch - '0';
  }
  if ('A' <= ch && ch <= 'F') {
    return ch - 'A' + 0xA;
  }
  return -1;
}

size_t promptHex(const __FlashStringHelper* prompt, uint8_t* buf, size_t bufLen, size_t expectLen) {
  size_t len;
  do {
    len = 0;
    const char* line = promptLine(prompt);
    for (size_t i = 0; i < bufLen; ++i) {
      int digit0 = parseHex(line[i*2]);
      int digit1 = parseHex(line[i*2+1]);
      if (digit0 < 0 || digit1 < 0) {
        break;
      }
      buf[i] = digit0 * 0x10 + digit1;
      ++len;
    }
  } while (expectLen > 0 && len != expectLen);
  return len;
}

void actSign() {
  //uint8_t input[64];
  //size_t inputLen = promptHex(F("input="), input, sizeof(input), 0);
  const uint8_t* input = THE_INPUT;
  size_t inputLen = sizeof(THE_INPUT);
  uint8_t sig[ndn::EcPrivateKey::MAX_SIG_LENGTH];

  ndn::NameLite keyName(nullptr, 0);
  ndn::EcPrivateKey key(pvtkey, keyName);
  unsigned long t0 = micros();
  int res = key.sign(input, inputLen, sig);
  unsigned long t1 = micros();
  Serial.print(F("duration="));
  Serial.println(t1 - t0);
  if (res == 0) {
    Serial.println(F("FAIL"));
    return;
  }
  printHex(sig, res);
}

void actVerify() {
  //uint8_t input[64];
  //size_t inputLen = promptHex(F("input="), input, sizeof(input), 0);
  const uint8_t* input = THE_INPUT;
  size_t inputLen = sizeof(THE_INPUT);
  uint8_t sig[ndn::EcPrivateKey::MAX_SIG_LENGTH];
  size_t sigLen = promptHex(F("sig="), sig, sizeof(sig), 0);

  ndn::EcPublicKey key(pubkey);
  unsigned long t0 = micros();
  bool res = key.verify(input, inputLen, sig, sigLen);
  unsigned long t1 = micros();
  Serial.print(F("duration="));
  Serial.println(t1 - t0);
  Serial.println(res ? F("res=OK") : F("res=BAD"));
}

void loop() {
  const char* act = promptLine(F("----ECDSA demo----\n"
                                 "1- show keys\n"
                                 "2- set pubkey\n"
                                 "3- verify\n"
                                 "4- set pvtkey\n"
                                 "5- sign\n"
                                 "action="));

  switch (act[0]) {
    case '1':
      Serial.print(F("pvtkey="));
      printHex(pvtkey, sizeof(pvtkey));
      Serial.print(F("pubkey="));
      printHex(pubkey, sizeof(pubkey));
      break;
    case '2':
      promptHex(F("pubkey="), pubkey, sizeof(pubkey), sizeof(pubkey));
      break;
    case '3':
      actVerify();
      break;
    case '4':
      promptHex(F("pvtkey="), pvtkey, sizeof(pvtkey), sizeof(pvtkey));
      break;
    case '5':
      actSign();
      break;
  }
}