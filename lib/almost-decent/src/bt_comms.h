#ifndef BT_COMMS_H
#define BT_COMMS_H

#include <functional>

void initBT();
void broadcastWeight(int gramsMultipliedByTen);
void setTareCallback(std::function<void()>);

#endif