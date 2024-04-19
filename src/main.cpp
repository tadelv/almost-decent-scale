#include <Arduino.h>
#include <EEPROM.h>
#include <almost-decent.h>

AlmostDecentScale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

const size_t eepromSize = sizeof(float);
EEPROMClass storage("scaleFactor");

void scaleLog(const char *message) {
  Serial.printf("[scale]: %s\n", message);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting scale");
  scale.m_logCallback = scaleLog;
  scale.initialize();
  if (scale.getState() == ScaleState::error) {
    Serial.println("failed to init scale ... will not continue");
    while (1);
  }

  storage.begin(eepromSize);
  float factor = storage.readFloat(0);
  if (factor == 0) {
    Serial.println("Scale not calibrated yet.");
    Serial.println("Press 'c' to start calibration.");
    return;
  }
  scale.setFactor(factor);
  scale.begin();
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
        break;
    }
  }
  scale.tick();
  delay(100);
}