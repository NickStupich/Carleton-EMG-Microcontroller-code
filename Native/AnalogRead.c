#include <AnalogRead.h>
#include <settings.h>

#define ANALOG_PIN_ZERO		0xE0034010

unsigned short analogRead(unsigned char pin){
	return ((*((int*)(ANALOG_PIN_ZERO) + pin)) >> 6) & 0x3FF;
}
