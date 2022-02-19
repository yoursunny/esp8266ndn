# esp8266ndn Bluetooth Low Energy transport for python-ndn

This is a Bluetooth Low Energy (BLE) transport module for [python-ndn](https://python-ndn.readthedocs.io/en/latest/), implemented with [bluepy](https://github.com/IanHarvey/bluepy) library.
It is tested on Raspberry Pi 4, and is known to work on other Raspberry Pi models.
See [this answer](https://raspberrypi.stackexchange.com/a/114588) regarding how to enable Bluetooth on Raspberry Pi running Ubuntu.

This transport works as the BLE client (central).
esp8266ndn `BleServerTransport` class works as the BLE server (peripheral).
The protocol is summarized in NDNts [@ndn/web-bluetooth-transport](https://github.com/yoursunny/NDNts/tree/main/packages/web-bluetooth-transport) package documentation.

## Installation and ndnping Demo

1. Install system-wide dependencies in a sudoer user:

    ```bash
    sudo apt install --no-install-recommends python3-dev python3-venv libglib2.0-dev
    curl -fsLS https://bootstrap.pypa.io/get-pip.py | sudo python3
    sudo pip install -U pip pipenv
    ```

2. Install local dependencies:

    ```bash
    pipenv install
    ```

3. Upload [BlePingServer](../../examples/BlePingServer) sketch to an ESP32 or nRF52 microcontroller.
   The MAC address of the microcontroller will be displayed on the serial console.

4. Run the ping client with the displayed address and address type:

    ```bash
    pipenv run python BlePingClient.py --addr 02:00:00:00:00:00 --addr-type public
    pipenv run python BlePingClient.py --addr 02:00:00:00:00:00 --addr-type random
    ```

## BLE-UDP Bridge

`BleUdpBridge.py` is a BLE-UDP bridge program.
It listens on a UDP port and connects to a BLE peripheral.

1. Start the bridge program:

    ```bash
    pipenv run python BleUdpBridge.py --addr 02:00:00:00:00:00 --addr-type public
    ```

2. In NFD, create a UDP face with NDNLPv2 fragmentation:

    ```bash
    nfdc face create udp4://127.0.0.1:6362 mtu 244 persistency permanent
    ```

3. Add route and send Interests:

    ```bash
    nfdc route add /example/esp8266/ble udp4://127.0.0.1:6362
    ndnping /example/esp8266/ble
    ```
