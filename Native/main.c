#include <lpc23xx.h>
#include <GPIOSetUp.h>
#include <Debug.h>
#include <time.h>
//#include <RLP.h>

int Start(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){	
	return 0;
}

int Stop(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	return 0;
}


int Init(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	
	
	Debug_uint(14);
	InitSPI();			//Initialize SPI on uC
	Init_SPI_wADS();		//Initialize SPI Communication B/W uC and ADS
	Initialize_eint1();     //Initialize Interrupts Using RLP framework

	return 0;

}

