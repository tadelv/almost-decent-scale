#ifndef ALMOST_DECENT_H
#define ALMOST_DECENT_H

#include <functional>

enum ScaleState : unsigned char {
  startup,
  ready,
  calibrating,
  measuring,
  taring,
  error
};

class AlmostDecent_;

class AlmostDecentScale {
private:
  AlmostDecent_ *m_internal;

public:
  AlmostDecentScale(int loadCellDoutPin, int loadCellSckPin);
  std::function<void(const char *)> m_logCallback = nullptr;
  std::function<void(const float)> m_factorCallback = nullptr;
  ScaleState getState();
  const char *getStateString();
  void initialize(bool ownThread = true); // creates own thread for processing
  void begin();                           // starts measuring
  void tick();                            // not using own thread
  void calibration();
  void _calibration();
  void setFactor(float factor);
  void tare();
};

void initBluetooth();
std::string formatWeightWithTimestamp(int weight, int minutes, int seconds,
                                      int milliseconds);
void sendWeight(int gramsMultipliedByTen);

#endif
