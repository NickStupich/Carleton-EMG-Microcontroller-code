#include <lpc23xx.h>
#include <GPIOSetUp.h>

int Start(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){	
	return 0;
}

int Stop(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	return 0;
}


int Init(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	unsigned int z;
	unsigned int data;
	int x = InitSPI();
	int z = InitADS;


	

	//FIO0CLR |= 1<<16;
	//for (z=0; z<1000000; z++){
	//writeToSPI0(0xEE);
	//data = readSPI0();
	//}	
	//FIO0SET |= 1<<16;
	return z;
}

