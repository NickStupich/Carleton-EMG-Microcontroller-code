#include <GPIOSetUp.h>
#include <lpc23xx.h>
#include <time.h>
#include <Debug.h>
#include <RLP.h>
#include <bluetooth.h>

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

void InitSPI(){

	PCONP |= 1<<8;
	
	//Set Peripheral clock frequency to 72MHz
	PCLKSEL0 |= 1<<16;
	PCLKSEL0 &= ~(1<<17);

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

	return;
	
}

//write to SPI0 MOSI pin 16
void SPI0_Write(unsigned char Data){

		S0SPDR = Data;
		while((S0SPSR & 0x80) == 0);
		return;
}

//read from SPI0 bit 17
unsigned char SPI0_Read(){

	unsigned char Data;
	
	//for dummy data write 0x00 to SPI to activate clock while reading the incoming data from ADS
	S0SPDR = 0x00;
	while (!(S0SPSR & 0x80));
	Data = S0SPDR;
	return Data;
	
}

//Initialize SPI communication between uC and ADS1298
#define RESET_DELAY_US		400//100
#define SPI_WRITE_DELAY		40//4
#define ADS_START_DELAY		80//8
void Init_SPI_wADS(){

	unsigned char datain[26];			//register information from ADS1298 stored in a 26 byte array corresponding to 26 registers
	int a;

	GPIOPinSetup(67, 1);  //SPI_Start Pin. Set high to begin converstions
	GPIOPinSetup(76, 1);  //SPI_Reset, Toggle the pin low then high to reset the chip
	//GPIOPinSetup(77, 0);  //SPI_DRDY pin as input
	
	DigitalOutput(67, 0); //Set SPI_Start Pin low, Do not begin conversions
	
	FIO0SET |= 1<<16;		  //Set CS pin to high
	
	//Reset ADS1298
	DigitalOutput(76, 1);	  //Set SPI_Reset pin high
	delay_us(RESET_DELAY_US);
	DigitalOutput(76, 0);	  //Set SPI_Reset pin low
	delay_us(RESET_DELAY_US);
	DigitalOutput(76, 1);	  //Set SPI_Reset pin high

	//Stop read data continuously mode
	FIO0CLR |= 1<<16;		//Set CS pin low
	delay_us(SPI_WRITE_DELAY);
	SPI0_Write(0x11);
	delay_us(SPI_WRITE_DELAY);
	FIO0SET |= 1<<16;       //Set CS pin high
	delay_us(SPI_WRITE_DELAY);

	
	//Write Registry Commands
	
	//Values for opcode 1 and opcode 2 as well as Registers 0-25 of ADS stored in this array
	unsigned char write_Array[28] = {0x40, 0x19, 0x92, 0x86, 0, 0x61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0F, 0, 0, 0, 0, 0};
	FIO0CLR |= 1<<16;
	
	for (a=0; a<28; a++){
		SPI0_Write(write_Array[a]);
		delay_us(SPI_WRITE_DELAY);
	}

	delay_us(SPI_WRITE_DELAY);
	FIO0SET |= 1<<16;   //set CS pin to high
	delay_us(SPI_WRITE_DELAY);

	
	//Read Registry Command
	FIO0CLR |= 1<<16;		//Set CS pin low
	delay_us(4);
	SPI0_Write(0x20);		//Opcode 1
	//delay_us(4);
	SPI0_Write(0x19);		//Opcode 2
	delay_us(4);
	
	int x;
	for (x=0; x<26; x++){
	datain[x] = SPI0_Read();
	}

	Debug_uint(datain[1]);

	delay_us(4);
	FIO0SET |= 1<<16;	//set CS pin to high
	delay_us(4);


	//Enable Read Data Continiously Mode
	FIO0CLR |= 1<<16;        //Set CS pin low
	delay_us(SPI_WRITE_DELAY);
	SPI0_Write(0x10);
	delay_us(SPI_WRITE_DELAY);
	FIO0SET |= 1<<16;       //Set CS pin high
	delay_us(SPI_WRITE_DELAY);
	
	
	DigitalOutput(67, 1);  //SPI_Start Pin set high to begin conversion
	delay_us(ADS_START_DELAY);
	
	
	
	return;

}

void Initialize_eint1(){
	
	/*
	PINSEL4 |= (1<<22);			//P2.10 is EINT1 Interrupt Enabled and attached to DRDY pin of ADS1298
	Debug_uint(PINSEL4);
	//VICIntSelect |= 1<<15;
	//Debug_uint(VICIntSelect);
	VICIntEnClr = (1<<15);		
	EXTMODE |= 1<<1;			//EINT1 is edge-sensitive
	Debug_uint(EXTMODE);
	EXTPOLAR |= 0xFD;		//Interrupt enabled on the falling edge
	Debug_uint(EXTPOLAR);
	VICVectAddr15 = (unsigned int) &isr_handler_eint1;  //assign ISR handler function to this VIC address
	Debug_uint(VICVectAddr15);
	VICVectPriority15 &= 0xFFFFFFF1;		//Set this interrupt to highest priority
	VICVectCntl0 |= 0x00000020;
	VICVectCntl15 |= 0x00000020;
	Debug_uint(VICVectPriority15);
	EXTINT |= 1<<1;						//clear the peripheral interrupt flag
	Debug_uint(EXTINT);
	VICIntEnable |= 1<<15;				//Enable interrupt in the VIC
	Debug_uint(VICIntEnable);

	//asm("msr CPSR_c, 0x13 \n");
	*/
	
	RLP_InterruptInputPinArgs  pinArgs;
	pinArgs.GlitchFilterEnable = RLP_FALSE;
	pinArgs.IntEdge = RLP_GPIO_INT_EDGE_LOW;
	pinArgs.ResistorState = RLP_GPIO_RESISTOR_DISABLED;
	
	unsigned int pin = 19;	//di2
	int success = RLPext->GPIO.EnableInterruptInputMode(pin, &pinArgs, RLP_ISR, 0);

}

void RLP_ISR(unsigned int Pin, unsigned int PinState, void* param)
{
	//Debug_uint(65);				//display that interrupt happened
	unsigned char DataArray[27];          //ADS1298 24bit channel data will be stored here
	//int b;
	
	FIO0CLR |= 1<<16;			//Set CS pin low
	

	/*	
	for (b=0; b<27; b++){
		 //DataArray[b] = SPI0_Read();
		S0SPDR = 0x00;
		while (!(S0SPSR & 0x80)); 
		DataArray[b] = S0SPDR;
	}*/
	
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[0] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[1] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[2] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[3] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[4] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[5] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[6] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[7] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[8] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[9] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[10] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[11] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[12] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[13] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[14] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[15] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[16] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[17] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[18] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[19] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[20] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[21] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[22] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[23] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[24] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[25] = S0SPDR;
	S0SPDR = 0x00;  while (!(S0SPSR & 0x80));       DataArray[26] = S0SPDR;
	
	/*
	int b;
	volatile char status;

		for (b=0; b<27; b++){
		 //DataArray[b] = SPI0_Read();
		S0SPDR = 0x00;
		//int i;
		
		
			asm volatile("mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				"mov r0,r0\n\t"
				);
		status = S0SPSR;
		
		while (!(S0SPSR & 0x80));
		DataArray[b] = S0SPDR;
	}
	*/
	FIO0SET |= 1<<16;			//Set CS pin high

	sendByte(DataArray[3]);
	sendByte(DataArray[4]);
	sendByte(DataArray[5]);


					
}

/*
//Interrupt Handling routine for external interrupt 3
__attribute__ ((interrupt("IRQ"))) void  isr_handler_eint1(void)   {
	
	//Debug_uint(65);				//display that interrupt happened
	//char DataArray[27];          //ADS1298 24bit channel data will be stored here
	//int b;

	VICIntEnClr = (1 << 15);	// Disable EINT1 in the VIC
	while(1){
		//DigitalOutput(77, 1);
		delay_us(1000);
		DigitalOutput(77,0);
		delay_us(1000);
		DigitalOutput(77,1);
		//delay_us(100000);
	}
	//delay_us(1000);
	//DigitalOutput(77,1);
	
	//FIO0CLR |= 1<<16;			//Set CS pin low

	//for (b=0; b<27; b++){
	//	 DataArray[b] = SPI0_Read();
	//}

	//FIO0SET |= 1<<16;			//Set CS pin low
					
	EXTINT |= 1<<1;				 //clear the peripheral interrupt flag
	VICIntEnable |= 1<<15;		//Enable EINT3 in the VIC
	VICVectAddr = 0;			//Acknowledge Interrupt
}

*/



	










