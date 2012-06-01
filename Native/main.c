#include <lpc23xx.h>
#include <GPIOSetUp.h>
#include <Debug.h>

int Start(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){	
	return 0;
}

int Stop(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	return 0;
}


int Init(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	int a;
	int b;
	int c;
	unsigned char z;
	unsigned int data;
	//Debug("RLP Init()");
	
	int x = InitSPI();			//Initialize SPI on uC
	Init_SPI_wADS();		//Initialize SPI Communication B/W uC and ADS

	Initialize_eint3();

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

	return z;
}

