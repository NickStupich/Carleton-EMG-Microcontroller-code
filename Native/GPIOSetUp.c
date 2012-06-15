#include <GPIOSetUp.h>
#include <lpc23xx.h>

#define PINSEL(x)	(*(volatile unsigned long *)(PINSEL_BASE_ADDR + 4*x))
#define PINMODE(x)	(*(volatile unsigned long *)(PINSEL_BASE_ADDR + 0x40 + 4*x))
#define FIODIR(x)   (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x20*x))
#define FIOMASK(x)  (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x10 + 0x20*x))
#define FIOSET(x)   (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x18 + 0x20*x))
#define FIOCLR(x)   (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x1C + 0x20*x))
#define FIOPIN(x)   (*(volatile unsigned long *)(FIO_BASE_ADDR + 0x14 + 0x20*x))
#define SPTCR		(*(volatile unsigned long *)(SPI0_BASE_ADDR + 0x10))

//setup a pin for GPIO input/output. 0 for input pin and 1 for output pin
void GPIOPinSetup(char pinNumber, char direction){

	//determining which PINSEL group the specified pin number belongs to.
	//if the number is 5 or 8 then do not proceed since it is not a selectable pin number according to datasheet 
	char setNumber = (pinNumber & 0xF0) >> 4;
	if(setNumber == 5) return;
	if(setNumber == 6) return;
	if(setNumber == 8) return;
	//select the GPIO function and mode for this pin number
	char setPin = (pinNumber & 0x0F);
	setPin = setPin*2;
	PINSEL(setNumber) &= ~(3<<setPin);
	PINMODE(setNumber) &= ~(3<<setPin); 

	//determine which FIODIR group the specified pin number belongs to in order to set direction
	setNumber = (pinNumber & 0xE0) >> 5;
	setPin = (pinNumber & 0x1F);

	if(direction == 1) FIODIR(setNumber) |= 1<<setPin;
	else FIODIR(setNumber) &= ~(1<<setPin);
	FIOMASK(setNumber) &= ~(1<<setPin);

}

//Outputs 1s or 0s to a specified GPIO enabled pin
void DigitalOutput(char pinNumber, char state){

	//determine which FIO group the specified pin number belongs to
	char setNumber = (pinNumber & 0xE0) >> 5;
	char setPin = (pinNumber & 0x1F);

	//set clear pin to 0 or set pin to 1
	if (state == 0) FIOCLR(setNumber) |= 1<<setPin;
	else FIOSET(setNumber) |= 1<<setPin;

}

//read value from a specified pin
int DigitalRead(char pinNumber){

	//determine which FIODIR group the specified pin number belongs to
	char setNumber = (pinNumber & 0xE0) >> 5;
	char setPin = (pinNumber & 0x1F);

	int readPin = FIOPIN(setNumber);
	readPin &= 1<<setPin;
    readPin = readPin>>setPin;
	return readPin;
	
}





	










