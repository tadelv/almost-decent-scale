#include <Arduino.h>
#include <HX711.h>
#include <NimBLEDevice.h>
#include <almost-decent.h>

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

AlmostDecentScale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting NimBLE Server");
  // pServer = NimBLEDevice::createServer();
  // scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  initBluetooth();
  scale.begin();
}

void loop() {
  sendWeight(101);
  delay(100);
}