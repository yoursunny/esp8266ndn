This example is tested with Heltec WiFi_LoRa_32 board and Heltec's LoRa library.
Parameters are tuned for use in US915 band.

Every board acts as both a ndnping server and a ndnping client.
The client controls the LED: it turns on the LED when sending an Interest, and turns off the LED when the Interest is satisfied or timed out.
You may enable/disable the client by pressing the builtin "PRG" button.
