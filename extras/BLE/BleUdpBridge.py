import argparse
import asyncio as aio
import typing as T

from ndn.transport.stream_socket import Face

from BleClientFace import BleClientFace


class BleBridgeProtocol(aio.DatagramProtocol):
    """NDN-BLE bridge protocol handler."""

    def __init__(self, face: Face, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)
        self._face = face
        self._face.callback = self._face_callback

    def connection_made(self, transport) -> None:
        self._transport = transport
        self._remote = None

    def datagram_received(self, data: bytes, addr: T.Any) -> None:
        self._remote = addr
        self._face.send(data)

    async def _face_callback(self, tlvType: int, wire: bytes):
        if self._remote is not None:
            self._transport.sendto(wire, self._remote)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='BLE-UDP bridge.')
    parser.add_argument('--addr', type=str, required=True, help='BLE address')
    parser.add_argument('--addr-type', default='public',
                        choices=['public', 'random'], help='BLE address type')
    parser.add_argument('--listen-addr', default='127.0.0.1',
                        help='UDP listen address', metavar='ADDR')
    parser.add_argument('--listen-port', type=int, default=6362,
                        help='UDP listen port', metavar='PORT')
    opts = parser.parse_args()

    face = BleClientFace(opts.addr, opts.addr_type)

    def create_protocol(*args, **kwargs):
        return BleBridgeProtocol(face, *args, **kwargs)

    loop = aio.new_event_loop()
    loop.run_until_complete(loop.create_datagram_endpoint(
        create_protocol, local_addr=(opts.listen_addr, opts.listen_port)))
    loop.run_until_complete(face.open())
    aio.ensure_future(face.run(), loop=loop)
    loop.run_forever()
