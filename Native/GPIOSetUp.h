#ifndef GPIOSetUp
#define GPIOSetUp

char GPIOPinSetup(char pinNumber, char direction);

char DigitalOutput(char pinNumber, char state);

int DigitalRead(char pinNumber);

int InitSPI();

#endif