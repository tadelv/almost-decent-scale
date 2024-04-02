#include "bt_message_building.h"
#include "bt_messages.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <vector>

unsigned char xorFunction(unsigned char a, unsigned char b)
{
  return a ^ b;
}

std::vector<unsigned char> buildWeightMessage(int weight, int minutes, int seconds, int milliseconds)
{
  std::vector<unsigned char> message;
  message.push_back(DecentHeader);
  message.push_back(BT_Message::WEIGHT);
  message.push_back(weight >> 8);
  message.push_back(weight);
  message.push_back(minutes & 0xff);
  message.push_back(seconds & 0xff);
  message.push_back(milliseconds & 0xff);
  message.push_back(0x00);
  message.push_back(0x00);

  unsigned char xor_result = 0;
  for (uint8_t idx = 0; idx < 4; idx++)
  {
    xor_result ^= message[idx];
  }

  message.push_back(xor_result);
  return message;
}