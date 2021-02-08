# esp8266ndn Bluetooth Low Energy transport for python-ndn

This directory contains a Bluetooth Low Energy (BLE) transport module for [python-ndn](https://python-ndn.readthedocs.io/en/latest/), and a simple ndnping client that demonstrates its usage.

This transport uses [bluepy](https://github.com/IanHarvey/bluepy) library.
It is tested on Raspberry Pi 4, and is known to work on Raspberry Pi 3, Raspberry Pi Zero W, and Linux laptops.
See [this answer](https://raspberrypi.stackexchange.com/a/114588) on how to enable Bluetooth on Raspberry Pi running Ubuntu.

This transport works as the BLE client (central).
esp8266ndn `BleServerTransport` class works as the BLE server (peripheral).
NDNts [@ndn/web-bluetooth-transport](https://www.npmjs.com/package/@ndn/web-bluetooth-transport) has a summary of the protocol.

## Demo Instructions

1. Install system-wide dependencies in a sudoer user:

    ```bash
    sudo apt install python3-dev python3-venv libglib2.0-dev
    curl -sL https://bootstrap.pypa.io/get-pip.py | sudo python3
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
    pipenv run python BlePingClient.py --address 02:00:00:00:00:00 --addr-type public
    pipenv run python BlePingClient.py --address 02:00:00:00:00:00 --addr-type random
    ```
