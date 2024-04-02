#ifndef BT_MESSAGE_BUILDING
#define BT_MESSAGE_BUILDING

#include <vector>

std::vector<unsigned char> buildWeightMessage(int weight, int minutes, int seconds, int milliseconds);

#endif
