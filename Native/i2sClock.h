//20 kHz signal using I2S in the SD card interface area, producing the signal at pin D1
PCONP |= (1<<27);//I2S power enabled - turned on above

PINSEL4 |= (0x3F << 22);	//select clock pin function
I2S_DAO = 0x0;				//

//for 12kHz signal 
PCLKSEL1 |= 3 << 22;
I2S_TXRATE = 753;

//for 20kHz signal
//I2S_TXRATE = 913;	
//PCLKSEL1 &= ~(3<<22);	//reset clock to 1/4
//counter limit for the I2S.  This should be 899 in theory, but setting it to 913 will make the notch 
//centered at exactly 120Hz
	