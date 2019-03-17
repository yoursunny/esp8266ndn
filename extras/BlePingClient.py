#!/usr/bin/python3

from bluepy import btle
import pyndn as ndn
import struct
import time

UUID_SVC = '099577e3-0788-412a-8824-395084d97391'
UUID_RX = 'cc5abb89-a541-46d8-a351-2f95a6a81f49'
UUID_TX = '972f9527-0d83-4261-b95d-b1b2fc73bde4'


class BleClientTransport(ndn.transport.Transport):
    def isLocal(self, connectionInfo):
        return False

    def isAsync(self):
        return False

    class MyDelegate(btle.DefaultDelegate):
        def __init__(self):
            btle.DefaultDelegate.__init__(self)
            self.rxHandle = None

        def handleNotification(self, cHandle, data):
            if cHandle == self.rxHandle:
                self.data = data

    def connect(self, connectionInfo, elementListener, onConnected):
        self.p = btle.Peripheral(connectionInfo[0], connectionInfo[1])
        self.p.setMTU(517)
        self.delegate = BleClientTransport.MyDelegate()
        self.p.setDelegate(self.delegate)

        service = self.p.getServiceByUUID(UUID_SVC)
        self.tx, = service.getCharacteristics(UUID_RX)
        self.rx, = service.getCharacteristics(UUID_TX)
        self.p.writeCharacteristic(
            self.rx.getHandle() + 1, struct.pack('<bb', 0x01, 0x00))
        self.delegate.rxHandle = self.rx.getHandle()

        self.el = elementListener
        if onConnected is not None:
            onConnected()

    def send(self, data):
        self.tx.write(data)

    def processEvents(self):
        self.delegate.data = None
        if self.p.waitForNotifications(0.2) and self.delegate.data is not None:
            self.el.onReceivedElement(self.delegate.data)

    def getIsConnected(self):
        return self.p is not None

    def close(self):
        self.p.disconnect()
        self.p = None


class PingClient(object):
    def __init__(self, face, prefix, interval):
        self.face = face
        self.prefix = ndn.Name(prefix)
        self.interval = interval
        self.lastTime = 0
        self.nSent = 0
        self.nRecv = 0

    def sendInterest(self):
        self.lastTime = time.time()
        name = ndn.Name(self.prefix)
        name.appendTimestamp(int(self.lastTime * 1000))
        self.lastName = name
        print('<I %s' % (name,))
        self.face.expressInterest(name, self.processData)
        self.nSent += 1

    def processData(self, interest, data):
        self.nRecv += 1
        satisfyRatio = float(self.nRecv)/float(self.nSent)*100
        if interest.getName() != self.lastName:
            print('>D %s late %0.1f%%' % (data.getName(), satisfyRatio))
        else:
            rtt = time.time() - self.lastTime
            print('>D %s %0.1fms %0.1f%%' %
                  (data.getName(), rtt * 1000, satisfyRatio))

    def run(self):
        while True:
            if self.lastTime + self.interval < time.time():
                self.sendInterest()
            self.face.processEvents()


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
        description='ndnping over Bluetooth Low Energy.')
    parser.add_argument('--addr', type=str, required=True, help='BLE address')
    parser.add_argument('--addr-type-random', action='store_true',
                        help='set address type to "random" instead of "public"')
    parser.add_argument('--prefix', type=str,
                        default='/example/esp32/ble/ping', help='NDN prefix')
    parser.add_argument('--interval', type=int,
                        default=1000, help='interval (ms)')
    args = parser.parse_args()

    transport = BleClientTransport()
    ci = (args.addr, btle.ADDR_TYPE_RANDOM if args.addr_type_random else btle.ADDR_TYPE_PUBLIC)
    face = ndn.Face(transport, ci)
    client = PingClient(face, args.prefix, float(args.interval) / 1000)
    client.run()