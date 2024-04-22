#ifdef BT_COMMS_FAKE
#include <Arduino.h>
#include <functional>

void initBT() {
  DEBUG_SERIAL.println(__func__);
}
void broadcastWeight(int gramsMultipliedByTen) {
  DEBUG_SERIAL.print(__func__);
  DEBUG_SERIAL.print(gramsMultipliedByTen);
  DEBUG_SERIAL.println();
}
void broadCastUnits(int unitsTimesThousand) {
  DEBUG_SERIAL.println(__func__);
}
void setTareCallback(std::function<void()>) {
  DEBUG_SERIAL.println(__func__);
}
void setCalibrateCallback(std::function<void()> callback) {
  DEBUG_SERIAL.println(__func__);
}
void setFactorCallback(std::function<void(float)> callback) {
  DEBUG_SERIAL.println(__func__);
}

#endif // BT_COMMS_FAKE