#ifndef GAIN_CONTROL
#define GAIN_CONTROL

#include "settings.h"

#if GAIN_CONTROL_ON == 0
//if gain control is off, instead of calling the function we just get something like value = value;
#define addGainValue(x, y) x

#else

unsigned short addGainValue(unsigned short value, unsigned char channel);
#endif

#endif