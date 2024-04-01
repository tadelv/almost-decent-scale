// #include <NimBLEDevice.h>

// service or (??) ID
// 0000FFF4-0000-1000-8000-00805F9B34FB
// https://decentespresso.com/decentscale_api

const char *decentScaleServiceId = "0000FFF4-0000-1000-8000-00805F9B34FB";
const short decentBLEModel = 0x03;
enum BT_Command : unsigned char
{
  CMD_LED = 0x0A,
  CMD_TIMER = 0x0B,
  CMD_TARE = 0x0F
};

enum BT_Message : unsigned char
{
  MSG_WEIGHT = 0xCE,
  MSG_WEIGHT_CHANGE = 0xCA // do not use or send, has been discontinued
};

enum ScaleState: unsigned char
{
  init,
  calibrating,
  measuring,
  tare
};

enum BTState: unsigned char
{
  off,
  scanning,
  connected
};


typedef struct _IndecentScale {
  ScaleState currentScaleState;
  BTState currentBtState;
} IndecentScale;

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>

int calcXor(int cmdtype, int cmdddata1, int cmdddata2 = 0)
{
  int xor_result;
  if (cmdddata2 == 0)
  {
    xor_result = 0x03 ^ cmdtype ^ cmdddata1;
  }
  else
  {
    // printf("data2: %d\n", cmdddata2);
    std::cout << "data2: " << std::hex << cmdddata2 << std::endl;
    xor_result = 0x03 ^ cmdtype ^ cmdddata1 >> 7 ^ cmdddata1 & 0xff ^ cmdddata2 >> 7 ^ cmdddata2 & 0xff;
  }

  std::stringstream stream;
  stream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << xor_result;
  std::string xor_string = stream.str();
  return xor_result;
}

std::string decent_scale_calc_xor(int cmdtype, int cmdddata)
{
  int xor_result = 0x03 ^ cmdtype ^ cmdddata ^ 0x00 ^ 0x00 ^ 0x00;
  std::stringstream stream;
  stream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << xor_result;
  std::string xor_string = stream.str();
  std::cout << "decent_scale_calc_xor for '" << cmdtype << "' '" << cmdddata << "' is '" << xor_string << "'" << std::endl;
  return xor_string;
}

std::string decent_scale_calc_xor4(int cmdtype, int cmdddata1, int cmdddata2)
{
  int xor_result = 0x03 ^ cmdtype ^ cmdddata1 ^ cmdddata2 ^ 0x00 ^ 0x00;
  std::stringstream stream;
  stream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << xor_result;
  std::string xor_string = stream.str();
  std::cout << "decent_scale_calc_xor4 for '" << cmdtype << "' '" << cmdddata1 << "' '" << cmdddata2 << "' is '" << xor_string << "'" << std::endl;
  return xor_string;
}

std::string decent_scale_make_command(int cmdtype, int cmddata, int cmddata2 = 0)
{
  std::string hex;
  if (cmddata2 == 0)
  {
    std::cout << "1 part decent scale command" << std::endl;
    hex = "03" + std::to_string(cmdtype) + std::to_string(cmddata) + "000000" + decent_scale_calc_xor(cmdtype, cmddata);
  }
  else
  {
    std::cout << "2 part decent scale command" << std::endl;
    hex = "03" + std::to_string(cmdtype) + std::to_string(cmddata) + std::to_string(cmddata2) + "0000" + decent_scale_calc_xor4(cmdtype, cmddata, cmddata2);
  }
  std::cout << "hex is '" << hex << "' for '" << cmdtype << "' '" << cmddata << "' '" << cmddata2 << "'" << std::endl;
  return hex;
}

unsigned char xorFunction(unsigned char a, unsigned char b)
{
  return a ^ b;
}

std::vector<unsigned char> buildWeightMessage(int weight, int minutes, int seconds, int milliseconds)
{
  std::vector<unsigned char> message;
  message.push_back(0x03);
  message.push_back(0xCE);
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

std::string formatWeightWithTimestamp(int weight, int minutes, int seconds, int milliseconds)
{

  std::vector<unsigned char> message = buildWeightMessage(weight, minutes, seconds, milliseconds);

  std::stringstream stream;

  for (auto b : message) {
    stream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)b;
  }

  return stream.str();
}