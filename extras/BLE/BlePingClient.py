import argparse

from ndn.app import NDNApp
from ndn.encoding import Component, Name
from ndn.security.keychain import KeychainDigest
from ndn.transport.stream_socket import Face
from ndn.types import InterestNack, InterestTimeout
from ndn.utils import timestamp

from BleClientFace import BleClientFace

parser = argparse.ArgumentParser(
    description='ndnping over Bluetooth Low Energy.')
parser.add_argument('--addr', type=str, required=True, help='BLE address')
parser.add_argument('--addr-type', default='public',
                    choices=['public', 'random'], help='BLE address type')
parser.add_argument('--prefix', type=str,
                    default='/example/esp8266/ble/ping', help='NDN prefix')
parser.add_argument('--interval', type=int, default=1000, help='interval (ms)')
args = parser.parse_args()

prefix = Name.from_str(args.prefix)
face = BleClientFace(args.addr, args.addr_type)
keychain = KeychainDigest()
app = NDNApp(face, keychain)


async def main():
    nRecv, nSent = 0, 0

    def computeSatisfyRatio():
        return float(nRecv)/float(nSent)*100

    while True:
        try:
            t0 = timestamp()
            iName = prefix + [Component.from_timestamp(t0)]
            nSent += 1
            print('<I %s' % (Name.to_str(iName),))
            dName, dMeta, dContent = await app.express_interest(iName, can_be_prefix=False, must_be_fresh=True, lifetime=1000)
            t1 = timestamp()
            nRecv += 1
            rtt = t1 - t0
            print('>D %s %0.1fms %0.1f%%' %
                  (Name.to_str(dName), rtt, computeSatisfyRatio()))
        except InterestNack as e:
            print('>N %s %0.1%%' % (e.reason, computeSatisfyRatio()))
        except InterestTimeout:
            print('>T %0.1f%%' % (computeSatisfyRatio(),))

app.run_forever(after_start=main())
