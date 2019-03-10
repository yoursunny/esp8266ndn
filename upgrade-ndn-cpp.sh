#!/bin/bash
# Upgrade ndn-cpp library.

NDNCPPPATH=$1
if ! [[ -f $NDNCPPPATH/README-NDN-CPP-Lite.md ]]; then
  echo ndn-cpp library not found. >/dev/stderr
  exit 3
fi
NDNCPPPATH=$(readlink -f $NDNCPPPATH)

rm -rf src/ndn-cpp/
mkdir -p src/ndn-cpp/
cd src/ndn-cpp

# copy code
cp -r $NDNCPPPATH/include/ndn-cpp/c ./
cp -r $NDNCPPPATH/include/ndn-cpp/lite ./
cp -r $NDNCPPPATH/src/c ./
cp -r $NDNCPPPATH/src/lite ./

# remove trailing whitespace
find -type f -exec sed -i -e 's/\s*$//' '{}' '+'

# delete transport
find -name '*transport*' -delete

# delete crypto
find -name 'ec-*-key*' -delete
find -name 'rsa-*-key*' -delete
rm c/util/crypto.c

# delete ndn-cpp-config include
for F in $(grep -lr '#include .*ndn-cpp-config.h' c lite); do
  sed -i '/#include .*ndn-cpp-config.h/ d' $F
done

# change #include to relative path
for F in $(grep -lr '<ndn-cpp/' c lite); do
  echo -n sed -i \''s~<ndn-cpp/\(.*\)>~\"'
  echo -n $(echo $F | awk 'BEGIN { FS="/" } { s=""; for(i=1;i<NF;++i) { s = s "../" }; print s }')
  echo '\1\"~'\' $F
done | bash

# delete round(x) macro
sed -i '/#define round/ d' c/common.h

# disable cryptosuite SHA256
sed -i '/#ifdef ARDUINO/ c\#if 0' lite/util/crypto-lite.cpp

# fix time library
sed -i -e '1 i\#include <Arduino.h>\nextern "C" {\n#define NDN_CPP_HAVE_TIME_H 1\n#define NDN_CPP_HAVE_GMTIME_SUPPORT 1' -e '/^ndn_getNowMilliseconds/ p' -e '/^ndn_getNowMilliseconds/ a\{\n  return millis();\n}' -e '/^ndn_getNowMilliseconds/,/}/ d' -e 's/timegm/mktime/' -e '$ a} // extern "C"' c/util/time.c
mv c/util/time.c c/util/time.cpp

# fix keyLocator->type
sed -i -e 's/(int)keyLocator->type < 0/keyLocator->type == (ndn_KeyLocatorType)-1/' c/encoding/tlv/tlv-key-locator.c

# create ndn-cpp-all.hpp
(
  echo '#ifndef ESP8266NDN_NDN_CPP_ALL_HPP'
  echo '#define ESP8266NDN_NDN_CPP_ALL_HPP'
  find -name '*.h' -or -name '*.hpp' | grep -v ndn-cpp-all | sed -e 's/^/#include "/' -e 's/$/"/'
  echo '#endif // ESP8266NDN_NDN_CPP_ALL_HPP'
) > ndn-cpp-all.hpp
