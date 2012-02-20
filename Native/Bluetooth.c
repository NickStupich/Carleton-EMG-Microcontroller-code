#include <Bluetooth.h>

#include <lpc23xx.h>

//give a channel so that we can easily swap between them
#define CHANNEL		1

//using the channel definition define the variables needed
#if CHANNEL == 0
#define UTHR		U0THR
#define ULSR		U0LSR
#elif CHANNEL == 1
#define UTHR		U1THR
#define ULSR		U1LSR
#elif CHANNEL == 2
#define UTHR		U2THR
#define ULSR		U2LSR
#else
#error UNDEFINED CHANNEL IN bluetooth.c
#endif

#define LSR_TEMT    (1<<6)
#define SERIAL_PORT_SENDING	!(ULSR & LSR_TEMT)

//overload to make things a little easier
#if HAS_STRING_FUNC == 1
void sendMsg(char* s){
	while(*s)
	{	
		sendByte(*s++);
	}
}
#endif

/* 
sends a single byte over bluetooth (using regular serial com interface)
while (SERIAL_PORT_SENDING);
  = while(!(U1LSR & LSR_TEMT));
 U1LSR is the line status register.  bit 7 of that is TEMT, which indicates whether the transmitter
 register (U1THR that we just wrote to) is empty.  LSR_TEMT = 0x40 = 0100 0000, so U1LSR&LSR_TEMT indicates 
 whether the U1THR register is empty (whether the byte to be sent has already been sent).  So 
 while(!transmit register is empty) will loop doing nothing until the previous byte has been sent, and the 
 transmit register is ready to send again.
 
 This could be done asynchronously using interrupts and a buffer.  That would likely be faster,
 but would also require more complexity and code size.  Currently the memory is 98% full with 6 channels, 
 and performance is not really a concern so i won't be doing this for now.  Another speed improvement could be increasing the 
 baud rate, but that may cause an increase in errors
 
 At 57600 baud rate, about 19% of processor time is spent on sending out data.  Not sure how much that would decrease if we went 
 to an async buffer refilling type situation
 */

void sendByte(char c){
	//wait for ready
	while (SERIAL_PORT_SENDING);
	
	//send
	UTHR = c;
	
	//make sure you check thats it's ready before sending anything else if not using this function
}
 
/* Some sending test functions*/
/*
void testBluetoothSend(){
	//char* msg = "RLP Bluetooth testing";
	char* msg = "ABCdef12345";
	
	Debug("Start of testBluetoothSend()");
	sendMsg(msg);
	Debug("Done sendMsg()");
}*/