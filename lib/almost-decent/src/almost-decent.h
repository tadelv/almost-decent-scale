#ifndef ALMOST_DECENT_H
#define ALMOST_DECENT_H

#include <functional>

enum ScaleState : unsigned char
{
  startup,
  ready,
  calibrating,
  measuring,
  error
};

class AlmostDecent_;

class AlmostDecentScale {
  private:
    AlmostDecent_ *m_internal;
  public: 
  AlmostDecentScale(int loadCellDoutPin, int loadCellSckPin);
  std::function<void(const char *)> m_logCallback = nullptr;
  ScaleState getState();
  void begin(bool ownThread = true); // creates own thread for processing
  void tick(); // not using own thread
  void calibration();
  void setFactor(float factor);
};


void initBluetooth();
std::string formatWeightWithTimestamp(int weight, int minutes, int seconds, int milliseconds);
void sendWeight(int gramsMultipliedByTen);

#endif