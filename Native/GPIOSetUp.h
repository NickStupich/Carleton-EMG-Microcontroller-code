#ifndef GPIOSetUp
#define GPIOSetUp

void GPIOPinSetup(char pinNumber, char direction);

void DigitalOutput(char pinNumber, char state);

int DigitalRead(char pinNumber);

#endif