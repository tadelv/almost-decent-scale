# Almost decent scale

Work in progress, a clone of [decent scale](https://decentespresso.com/decentscale_api) for home use

## BOM:
- ESP32 or ESP32s3
- HX711
- Load cell

## Calibrating over Bluetooth:

The scale supports calibrating over bluetooth. To calibrate the scale, send the `0x03F1` command to `36F5` which will activate the calibration mode.

Once in calibration, the process is the same as when calibrating over Serial port.
- remove all weights and wait for the scale to tare (7 seconds)
- add a known weight
- read the value from the weight characteristic `FFF4`
- calculate the factor: (units_read_from_char / known_weight)
- write `0x03F2XXXXXXXX` to `36F5` where XX is the uint32 representation of the factor multiplied by 1000
- scale should be calibrated now


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