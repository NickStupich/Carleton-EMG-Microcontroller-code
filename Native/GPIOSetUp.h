#ifndef GPIOSetUp
#define GPIOSetUp

void GPIOPinSetup(char pinNumber, char direction);

void DigitalOutput(char pinNumber, char state);

int DigitalRead(char pinNumber);

int InitSPI();

void SPI0_Write(unsigned char Data);

void SPI0_Read();

unsigned char Init_SPI_wADS();

void Initialize_eint3();
#endif