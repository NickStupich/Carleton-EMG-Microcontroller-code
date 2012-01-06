#include "lpc23xx.h"
#include "settings.h"

#ifndef TIME
#define TIME

#define ONE_SECOND		18000000 //number of timer ticks/second

#define time_t			unsigned long

#if makeInline == 1
#define readTimer()		((unsigned int)T0TC)
#else
time_t readTimer();
#endif

#endif