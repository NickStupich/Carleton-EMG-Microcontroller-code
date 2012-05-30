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

int InitSPI(){

	PCONP |= 1<<8;
	
	//Set Peripheral clock frequency to 72MHz/8
	PCLKSEL0 |= (3<<17);

	// Configure SPI PINs. 
	PINSEL0 |= 0xC0000000;			//Set P0.15 to be SPI clock
	GPIOPinSetup(16,1);			//Set P0.16 to be GPIO for SPI_CS/SSEL
	DigitalOutput(16,1);		//Disabled Chip Select
	PINSEL1 |= 15<<2;			//Set P0.17, P0.18 to be  MISO and MOSI
	PINMODE0 &= ~(3<<31);		
	PINMODE1 &= ~63;			
	
	//configure SPI Control Register
	S0SPCR &= ~(1<<2);      //SPI Receives 8 bits per data transfer
	S0SPCR |= 1<<3;			//CPHA is set to 1
	S0SPCR &= ~(1<<4);			//clock polarity set to active low
	S0SPCR |= 1<<5;			//SPI in master mode
	S0SPCR &= ~(1<<6);		//MSB transferred first
	S0SPCR |= (1<<7);      //UnMask SPI interupts


	//Configure SPI clock frequency PCLKSEL0/(SPI Clock Counter)
	S0SPCCR = 0x08;

	//SPI Test Register
	//SPTCR |= 0xFE;

	return S0SPCR;
	
}


void SPI0_Read_Write(unsigned int Data){
	//FIO0CLR |= 1<<16;
	
	//for dummy data (0xAA), write 0x00 to SPI to activate clock while reading the incoming data from ADS
	if(Data == 0xAA){
	S0SPDR = 0x00;
	while((S0SPSR & 0x80) == 0);
	int read_Data = S0SPDR;       //read the incoming data from the ADS and store in variable
	return read_Data;

	else {
		S0SPDR = Data
		

	
	return;
	//unsigned int dummy = S0SPDR;
	//FIO0SET |= 1<<16;
}

unsigned int readSPI0(){

	unsigned int Data;
	//S0SPDR = 0xFF;
	while (!(S0SPSR & 0x80));
	Data = S0SPDR;
	return Data;
	
}

int InitADS(){

	unsigned char datain[2]

	GPIOPinSetup(67, 1);  //SPI_Start Pin. Set high to begin converstions
	GPIOPinSetup(76, 1);  //SPI_Reset, Toggle the pin low then high to reset the chip
	GPIOPinSetup(77, 0);  //SPI_DRDY pin as input
	
	DigitalOutput(67, 1); //Set SPI_Start Pin low, Do not begin conversions
	
	FIO0SET |= 1<<16;		  //Set CS pin to high
	
	//Reset ADS1298
	DigitalOutput(76, 1);	  //Set SPI_Reset pin high
	Delay_us(100);
	DigitalOutput(76, 0);	  //Set SPI_Reset pin low
	Delay_us(100);
	DigitalOutput(76, 1);	  //Set SPI_Reset pin high

	//Stop read data continuously mode
	FIO0CLR |= 1<<16;		//Set CS pin low
	delay_us(4);
	writetoSPI(0x11);
	delay_us(4);
	FIO0SET |= 1<<16;       //Set CS pin high
	delay_us(4);

	//Read Registry Command
	FIO0CLR |= 1<<16;		//Set CS pin low
	delay_us(4);
	writetoSPI(0x20);		//Opcode 1
	writetoSPI(0x01);		//Opcode 2

	datain[0] = 





	
	










