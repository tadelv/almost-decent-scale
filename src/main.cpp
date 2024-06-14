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
  LOGSerial.printf("[scale]: %s\n", message);
}

void saveScaleFactor(float factor) {
  LOGSerial.printf("saving factor %.5f for next time\n", factor);
  storage.writeFloat(0, factor);
  storage.commit();
}

void setup() {
  #ifdef ESP32S3_SLOWDOWN
  setCpuFrequencyMhz(80);
  yield();
  #endif
  
  delay(2 * 1000);
  LOGSerial.begin(115200);
  LOGSerial.println("Starting scale");
  #ifdef ESP32S3_SLOWDOWN
  LOGSerial.printf("cpufreq: %d\n", getCpuFrequencyMhz());
  LOGSerial.printf("apbfreq: %d\n", getApbFrequency());
  LOGSerial.printf("xtalfreq: %d\n", getXtalFrequencyMhz());
  #endif
  scale.m_logCallback = scaleLog;
  scale.m_factorCallback = saveScaleFactor;
  scale.initialize();
  if (scale.getState() == ScaleState::error) {
    LOGSerial.println("failed to init scale ... will not continue");
    while (1);
  }

  storage.begin(eepromSize);
  float factor = storage.readFloat(0);
  LOGSerial.printf("stored factor: %f\n", factor);
  scale.setFactor(factor);
  scale.begin();
  LOGSerial.printf("scale state: %s\n", scale.getStateString());
}

void loop() {
  if (LOGSerial.available()) {
    char cmd[3];
    LOGSerial.readBytes(cmd, 2);
    switch (cmd[0]) {
      case 'c':
        scale.calibration();
        break;
      case 'f':
      {
        LOGSerial.println("enter scale factor");
        while (LOGSerial.available() == 0) {};
        String factorString = LOGSerial.readStringUntil('\n');
        LOGSerial.printf("reading is: %s\n", factorString.c_str());
        float factorFloat = factorString.toFloat();
        LOGSerial.printf("factor is: %f\n", factorFloat);
        if (factorFloat == 0)
        {
          break;
        }
        scale.setFactor(factorFloat);
      }
        break;
      case 't':
        scale.tare();
        break;
    }
  }
  scale.tick();
}