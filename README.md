# Almost decent scale

Work in progress, a clone of [decent scale](https://decentespresso.com/decentscale_api) for home use

## BOM:
- ESP32 or ESP32s3
- HX711
- Load cell

## TODO:

- conform to DE scale BT protocol (tare, led, version, etc) ✔️ looks ok
- Add own thread logic
- Calibration over BT ✔️
- DE1 tare command handling ✔️
- esp32-s3 target ✔️
- (maybe) web server
- esp32-s2 mini target <- cancelled, only has wifi, maybe one day s3-mini


Use platformio.ini to define pins for your setup. Also if you want to use fake BT for some reason, add `-D BT_COMMS_FAKE`, otherwise use `-D BT_COMMS_NIMBLE`.

Additionally, I had to slow down my esp32s3 devboard - use `-D ESP32S3_SLOWDOWN` to reduce clock speed to 80MHz.