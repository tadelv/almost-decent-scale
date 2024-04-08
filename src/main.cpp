#include <Arduino.h>
#include <almost-decent.h>

const int LOADCELL_DOUT_PIN = 20;
const int LOADCELL_SCK_PIN = 19;

AlmostDecentScale scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

void scaleLog(const char *message) {
  Serial.printf("[scale]: %s\n", message);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting scale");
  scale.m_logCallback = scaleLog;
  scale.begin();
  if (scale.getState() == ScaleState::error) {
    Serial.println("failed to init scale ... will not continue");
    while (1);
  }
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
        String factorString = Serial.readString();
        if (factorString.toFloat() > 0) {
          scale.setFactor(factorString.toFloat());
        }
        break;
    }
  }
  scale.tick();
  delay(100);

}