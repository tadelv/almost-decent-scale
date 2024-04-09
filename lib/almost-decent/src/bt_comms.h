#ifndef BT_COMMS_H
#define BT_COMMS_H

#include <functional>

void initBT();
void broadcastWeight(int gramsMultipliedByTen);
void broadCastUnits(int unitsTimesThousand);
void setTareCallback(std::function<void()>);
void setCalibrateCallback(std::function<void()> callback);
void setFactorCallback(std::function<void(float)> callback);

#endif