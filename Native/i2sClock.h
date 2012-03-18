//20 kHz signal using I2S in the SD card interface area, producing the signal at pin D1
PCONP |= (1<<27);//I2S power enabled - turned on above

PINSEL4 |= (0x3F << 22);	//select clock pin function
I2S_DAO = 0x0;				//select master mode

//for 12kHz signal 
PCLKSEL1 |= 3 << 22;	//peripheral clock set to 1/8 main clock (otherwise we can't divide enough for 12kHz - min is ~17.8?kHz
I2S_TXRATE = 753;		//set clock divider for 12kHz (ish - actually tuned experimentally)