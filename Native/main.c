#include <lpc23xx.h>
#include <GPIOSetUp.h>
#include <Debug.h>
#include <time.h>

int Start(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){	
	return 0;
}

int Stop(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	return 0;
}


int Init(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	//int a;
	//int b;
	//unsigned char z;
	//unsigned int data;
	//Debug("RLP Init()");
	
	//InitSPI();			//Initialize SPI on uC
	
	Debug_uint(14);
	GPIOPinSetup(77, 1);
	DigitalOutput(77, 0);
	Initialize_eint1();
	delay_us(100000);
	DigitalOutput(77,1);
	
	/*
	while(1){
		DigitalOutput(77, 1);
		delay_us(10000);
		DigitalOutput(77,0);
		delay_us(1000);
	}
	//Init_SPI_wADS();		//Initialize SPI Communication B/W uC and ADS
	*/
	

	/*
	for (a=0; a<100000000; a++){
		
		while (DigitalRead(77) == 0){
			FIO0CLR |= 1<<16;
			for (b=0; b<27; b++){
				z = SPI0_Read();
			}
			FIO0SET |= 1<<16; 
		}
	}
	*/

	return 0;
}

