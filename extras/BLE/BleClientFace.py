import asyncio as aio
import struct
from concurrent.futures import ThreadPoolExecutor

from bluepy import btle
from ndn.encoding import parse_tl_num
from ndn.transport.stream_socket import Face

UUID_SVC = '099577e3-0788-412a-8824-395084d97391'
UUID_CS = 'cc5abb89-a541-46d8-a351-2f95a6a81f49'
UUID_SC = '972f9527-0d83-4261-b95d-b1b2fc73bde4'


class BleClientFace(Face):
    class MyDelegate(btle.DefaultDelegate):
        def __init__(self):
            btle.DefaultDelegate.__init__(self)
            self.scHandle = None
            self.rxQueue = []

        def handleNotification(self, cHandle: int, data: bytes):
            if cHandle == self.scHandle:
                self.rxQueue.append(data)

    def __init__(self, addr: str, addrType: str):
        Face.__init__(self)
        self.addr = addr
        self.addrType = addrType
        self.txQueue = []

    async def open(self):
        self.p = btle.Peripheral(self.addr, self.addrType)
        self.p.setMTU(517)
        self.delegate = BleClientFace.MyDelegate()
        self.p.setDelegate(self.delegate)

        service = self.p.getServiceByUUID(UUID_SVC)
        self.cs, = service.getCharacteristics(UUID_CS)
        self.sc, = service.getCharacteristics(UUID_SC)
        self.p.writeCharacteristic(
            self.sc.getHandle() + 1, struct.pack('<bb', 0x01, 0x00))
        self.delegate.scHandle = self.sc.getHandle()

        self.running = True

    def shutdown(self):
        if not self.running:
            return
        self.running = False
        self.p.disconnect()
        self.p = None
        self.cs = None
        self.sc = None
        self.delegate = None

    def send(self, data: bytes):
        self.txQueue.append(data)

    async def run(self):
        self.loop = aio.get_event_loop()
        self.pool = ThreadPoolExecutor()
        while self.running:
            await aio.wrap_future(self.pool.submit(self._once))

    def _once(self):
        for pkt in self.txQueue:
            self.cs.write(pkt)
        self.txQueue.clear()

        self.delegate.data = None
        if not self.p.waitForNotifications(0.1):
            return
        for pkt in self.delegate.rxQueue:
            wire = memoryview(pkt)
            tlvType, sizeofTlvType = parse_tl_num(wire)
            aio.ensure_future(self.callback(tlvType, wire), loop=self.loop)
        self.delegate.rxQueue.clear()
