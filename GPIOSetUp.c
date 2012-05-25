#include <lpc23xx.h>

//char x;
//define a macro in order to select the right PINSEL 0-9 of the uC at runtime
//#define PINSEL(x)  (*(volatile unsigned long *)(PINSEL_BASE_ADDR + 4*x))
//#define PINMODE(x) (*(volatile unsigned long *)(PINSEL_BASE_ADDR + 0x40 + 4*x))


int GPIOOutputPinSetup(int pinNumber){
/*	
	determining which PINSEL group the specified pin number belongs to.
	if the number is 5 or 8 then do not proceed since it is not a selectable pin number according to datasheet 
	char setNumber = (pinNumber & 0xF0) >> 4;
	if(setNumber == 5) return 0;
	if(setNumber == 6) return 0;
	if(setNumber == 8) return 0;
	x = setNumber;
	//select the GPIO function and mode for this pin number
	char setPIN = (pinNumber & 0x0F);
	setPIN = setPIN*2;
	PINSEL(x) &= ~(3<<setPIN);
	PINMODE(x) &= ~(3<<setPIN); 
	*/
	return 0;
}

/* void GPIOSetDigitalOutput(char pinNumber, char State){ 

	




	
	

