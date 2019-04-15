#!/usr/bin/python3

from bluepy import btle
import pyndn
import struct
import time

UUID_SVC = '099577e3-0788-412a-8824-395084d97391'
UUID_RX = 'cc5abb89-a541-46d8-a351-2f95a6a81f49'
UUID_TX = '972f9527-0d83-4261-b95d-b1b2fc73bde4'


class BleClientTransport(pyndn.transport.Transport):
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


class BridgeRecipient(object):
    def __init__(self, other):
        self.other = other

    def onReceivedElement(self, element):
        self.other.send(element)


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
        description='NFD <-> Bluetooth Low Energy proxy.')
    parser.add_argument('--addr', type=str, required=True, help='BLE address')
    parser.add_argument('--addr-type-random', action='store_true',
                        help='set address type to "random" instead of "public"')
    parser.add_argument('--uuid16', action='store_true',
                        help='use 16-bit UUIDs instead of 128-bit UUIDs')
    args = parser.parse_args()

    if args.uuid16:
        UUID_SVC = 'FF00'
        UUID_RX = 'FF01'
        UUID_TX = 'FF02'

    transport0 = BleClientTransport()
    ci0 = (args.addr, btle.ADDR_TYPE_RANDOM if args.addr_type_random else btle.ADDR_TYPE_PUBLIC)
    transport1 = pyndn.transport.UnixTransport()
    ci1 = pyndn.transport.UnixTransport.ConnectionInfo("/var/run/nfd.sock")

    transport0.connect(ci0, BridgeRecipient(transport1), None)
    transport1.connect(ci1, BridgeRecipient(transport0), None)

    while True:
        transport0.processEvents()
        transport1.processEvents()
        time.sleep(0.1)
