#ifndef GPIOSetUp
#define GPIOSetUp

void GPIOPinSetup(char pinNumber, char direction);

void DigitalOutput(char pinNumber, char state);

int DigitalRead(char pinNumber);

int InitSPI();

void writeToSPI0(unsigned int Data);

unsigned int readSPI0();

#endif