#!/bin/bash
# Upgrade ndn-lite library.

NDNLITEPATH=$1
if ! [[ -f $NDNLITEPATH/ndn-constants.h ]]; then
  echo ndn-lite library not found. >/dev/stderr
  exit 3
fi
NDNLITEPATH=$(readlink -f $NDNLITEPATH)

rm -rf src/ndn-lite/
mkdir -p src/ndn-lite/
cd src/ndn-lite

# copy code
cp -r $NDNLITEPATH/* ./

# remove space before tab, trailing whitespace
find -type f -exec sed -i -e 's/^\s*\t/\t/g' -e 's/\s*$//' '{}' '+'

# delete name-splay: conflicts with name-tree
rm forwarder/name-splay.*

# delete app-support/security-sign-on: depends on uECC
rm -rf app-support/secure-sign-on

# replace uECC with vendor/uECC
rm -rf security/default-backend/sec-lib/micro-ecc
sed -i \
  -e 's|sec-lib/micro-ecc/uECC.h|../../../vendor/uECC.h|' \
  security/default-backend/ndn-lite-default-ecc-impl.h
sed -i \
  -e '/uECC_Curve curve/ d' \
  -e 's|switch\s*(\([a-z]*\)_type)|if (\1_type != NDN_ECDSA_CURVE_SECP256R1)|' \
  -e '/case NDN_ECDSA_CURVE_/,/default/ d' \
  -e 's|, curve||' \
  -e 's|uECC_curve_public_key_size(curve)|(uECC_BYTES*2)|' \
  -e 's|uECC_curve_private_key_size(curve)|uECC_BYTES|' \
  -e '/uECC_verify/ s|, input_size||' \
  -e '/uECC_sign/ s|, input_size||' \
  security/default-backend/ndn-lite-default-ecc-impl.c

# delete tinycrypt RNG
rm security/default-backend/sec-lib/tinycrypt/tc_ecc_platform_specific.*
sed -i \
  -e '/tc_ecc_platform_specific/ d' \
  security/default-backend/sec-lib/tinycrypt/tc_ecc.c
