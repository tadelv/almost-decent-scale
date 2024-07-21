# Almost decent scale

Work in progress, a clone of [decent scale](https://decentespresso.com/decentscale_api) for home use

## BOM:
- ESP32 or esp32-s3-mini
- HX711
- Load cell ([either 500g or 750g](https://www.aliexpress.com/item/32670225988.html)) - take care of screw location / orientation, this will affect your enclosure choice.
- some M3 flathead and countersunk screws you probably have lying around
- [M3 threaded inserts](https://www.aliexpress.com/item/1005002897983868.html) if you're going with the 500g design.

## Designs / Enclosures

Currently, two enclosure designs are available in this repo. A build for esp32-s3-mini and a build for esp32, provided by [@randomcoffeesnob](https://github.com/randomcoffeesnob).
The design for esp32-s3-mini is still under active development, while the esp32 and 500g load cell is ready to go!

Pick the one you want, based on the equipment you have. *But be wary, esp32-s3-mini design is really not finished yet!*

## Calibrating over Bluetooth:

The scale supports calibrating over bluetooth. To calibrate the scale, send the `0x03F1` command to `36F5` which will activate the calibration mode.

Once in calibration, the process is the same as when calibrating over Serial port.
- remove all weights and wait for the scale to tare (7 seconds)
- add a known weight
- read the value from the weight characteristic `FFF4`
- calculate the factor: (units_read_from_char / known_weight)
- write `0x03F2XXXXXXXX` to `36F5` where XX is the uint32 representation of the factor multiplied by 1000 (i.e. `0x03F20000073A` for factor 1.85).
- scale should be calibrated now


## TODO:

- conform to DE scale BT protocol (tare, led, version, etc) ✔️ looks ok
- Add own thread logic
- Calibration over BT ✔️
- DE1 tare command handling ✔️
- esp32-s3 target ✔️
- (maybe) web server
- esp32-s2 mini target <- cancelled, only has wifi, maybe one day s3-mini
- esp32-s3-mini ✔️


Use platformio.ini to define pins for your setup. Also if you want to use fake BT for some reason, add `-D BT_COMMS_FAKE`, otherwise use `-D BT_COMMS_NIMBLE`.

Additionally, I had to slow down my esp32s3 devboard - use `-D ESP32S3_SLOWDOWN` to reduce clock speed to 80MHz.
