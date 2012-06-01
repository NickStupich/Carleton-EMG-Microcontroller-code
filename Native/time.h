#include "lpc23xx.h"
#include "settings.h"

#ifndef TIME
#define TIME

#define ONE_SECOND		18000000 //number of timer ticks/second

#define time_t			unsigned long

#if makeInline == 1
#define readTimer()		((time_t)T0TC)
#else
time_t readTimer();
#endif

void delay_us(unsigned int delay);

#endif