#include "time.h"

#if makeInline != 1
time_t readTimer()
{
	return (unsigned int)T0TC;
}

#endif

//Delay function (in MicroSeconds)
void delay_us(unsigned int delay){

	delay *= 18;					//Peripheral clock is 18MHz, therefore delay*18 will represent delay in terms of peripheral clock cylces
	
	//Handles delay despite rollover of T0TC variable
	unsigned long lastTime = T0TC;  
	unsigned long timeout = delay;

	while ((T0TC - lastTime) < timeout);
}