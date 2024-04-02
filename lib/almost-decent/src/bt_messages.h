#ifndef BT_MESSAGES_H
#define BT_MESSAGES_H


const unsigned char DecentHeader = 0x03;

enum BT_Command : unsigned char
{
  LED = 0x0A,
  TIMER = 0x0B,
  TARE = 0x0F
};

enum BT_Message : unsigned char
{
  WEIGHT = 0xCE,
  WEIGHT_CHANGE = 0xCA // do not use or send, has been discontinued
};


#endif
