#ifndef GPIOSetUp
#define GPIOSetUp

void GPIOPinSetup(char pinNumber, char direction);

void DigitalOutput(char pinNumber, char state);

int DigitalRead(char pinNumber);

void InitSPI();

void SPI0_Write(unsigned char Data);

unsigned char SPI0_Read();

void Init_SPI_wADS();

void Initialize_eint1();


void  isr_handler_eint1(void) __attribute__ ((interrupt ("IRQ")));

#endif