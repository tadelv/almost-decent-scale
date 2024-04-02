#ifndef ALMOST_DECENT_H
#define ALMOST_DECENT_H

// HX711 scale;
// static NimBLEServer *pServer;

enum ScaleState : unsigned char
{
  startup,
  calibrating,
  measuring,
  tare
};

class AlmostDecent_;

class AlmostDecentScale {
  private:
    AlmostDecent_ *m_internal;
  public: 
  AlmostDecentScale(int loadCellDoutPin, int loadCellSckPin);
  ScaleState getState();
  void begin(bool ownThread = true); // creates own thread for processing
  void tick(); // not using own thread
};


void initBluetooth();
std::string formatWeightWithTimestamp(int weight, int minutes, int seconds, int milliseconds);
void sendWeight(int gramsMultipliedByTen);

#endif