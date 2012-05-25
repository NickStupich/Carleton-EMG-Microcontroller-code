#include <lpc23xx.h>
#include <GPIOSetUp.h>

int Start(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){	
	return 0;
}

int Stop(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	return 0;
}


int Init(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	//int x;
	//char y;
	//x = 68;
	//y = GPIOPinSetup(x, 0);
	//y = DigitalOutput(x, 1);
	//int z = DigitalRead(x);


	int x = InitSPI();

	return ;
}

