#include <AnalogRead.h>
#include <settings.h>
#define ANALOG_PIN_ZERO		0xE0034010

//should set up the analog input to only use an 8 bit ADC if only this is being used

#if DOSAWTOOTH == 1
//sawtooth wave for testing
char result = 8;
unsigned char analogByteRead(unsigned char pin){
	result++;
	result %= 12;
	return result;
}
#else

/*
unsigned char analogByteRead(unsigned char pin){
	return ((*((int*)(ANALOG_PIN_ZERO) + pin)) >> 8) & 0xFF;
}
*/

unsigned short analogRead(unsigned char pin){
	return ((*((int*)(ANALOG_PIN_ZERO) + pin)) >> 6) & 0x3FF;
}

#endif
