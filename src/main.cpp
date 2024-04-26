#include <Arduino.h>
#include <EEPROM.h>
#include <almost-decent.h>
#ifdef ESP32S3_SLOWDOWN
#include "soc/rtc.h"
#endif


AlmostDecentScale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

const size_t eepromSize = sizeof(float);
EEPROMClass storage("scaleFactor");

void scaleLog(const char *message) {
  Serial.printf("[scale]: %s\n", message);
}

void setup() {
  #ifdef ESP32S3_SLOWDOWN
  setCpuFrequencyMhz(80);
  yield();
  #endif
  
  delay(2 * 1000);
  Serial.begin(115200);
  Serial.println("Starting scale");
  #ifdef ESP32S3_SLOWDOWN
  Serial.printf("cpufreq: %d\n", getCpuFrequencyMhz());
  Serial.printf("apbfreq: %d\n", getApbFrequency());
  Serial.printf("xtalfreq: %d\n", getXtalFrequencyMhz());
  #endif
  scale.m_logCallback = scaleLog;
  scale.initialize();
  if (scale.getState() == ScaleState::error) {
    Serial.println("failed to init scale ... will not continue");
    while (1);
  }

  storage.begin(eepromSize);
  float factor = storage.readFloat(0);
  Serial.printf("stored factor: %f\n", factor);
  scale.setFactor(factor);
  scale.begin();
  Serial.printf("scale state: %s\n", scale.getStateString());
}

void loop() {
  if (Serial.available()) {
    char cmd[3];
    Serial.readBytes(cmd, 2);
    switch (cmd[0]) {
      case 'c':
        scale.calibration();
        break;
      case 'f':
      {
        Serial.println("enter scale factor");
        while (Serial.available() == 0) {};
        String factorString = Serial.readStringUntil('\n');
        Serial.printf("reading is: %s\n", factorString.c_str());
        float factorFloat = factorString.toFloat();
        Serial.printf("factor is: %f\n", factorFloat);
        if (factorFloat == 0)
        {
          break;
        }
        scale.setFactor(factorFloat);
        Serial.println("saving factor for next time");
        storage.writeFloat(0, factorFloat);
        storage.commit();
      }
        break;
      case 't':
        scale.tare();
        break;
    }
  }
  scale.tick();
}