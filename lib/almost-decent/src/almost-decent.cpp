#include "bt_message_building.h"
#include "bt_comms.h"
#include "bt_messages.h"

// https://decentespresso.com/decentscale_api

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>

class AlmostDecent_ {

};

void initBluetooth() {
  initBT();
}

std::string formatWeightWithTimestamp(int weight, int minutes, int seconds, int milliseconds)
{

  std::vector<unsigned char> message = buildWeightMessage(weight, minutes, seconds, milliseconds);

  std::stringstream stream;

  for (auto b : message) {
    stream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)b;
  }

  return stream.str();
}

void sendWeight(int gramsMultipliedByTen) {
  broadcastWeight(gramsMultipliedByTen);
}