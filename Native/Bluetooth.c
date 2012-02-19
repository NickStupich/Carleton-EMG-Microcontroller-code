#include <Bluetooth.h>

#include <lpc23xx.h>

//give a channel so that we can easily swap between them
#define CHANNEL		2

//using the channel definition define the variables needed
#if CHANNEL == 0
#define UTHR		U0THR
#define ULSR		U0LSR
#elif CHANNEL == 2
#define UTHR		U2THR
#define ULSR		U2LSR
#else
#error UNDEFINED CHANNEL IN bluetooth.c
#endif

#define LSR_TEMT    (1<<6)
#define SERIAL_PORT_SENDING	!(ULSR & LSR_TEMT)



/*
 Sends a message over bluetooth.  Flow is commented in code.
 
 Explanation of what goes on here when sending a single char:
 See page 434 of user.manual.lpc23xx.pdf for references
 
 
 U0THR = (*s++);
 set U0THR to be the value of the char to send, and increment the pointer to the next char in the array
 U0THR is the send register, with all 8 bits Write Data
 
 while (SERIAL_PORT_SENDING);
  = while(!(U1LSR & LSR_TEMT));
 U1LSR is the line status register.  bit 7 of that is TEMT, which indicates whether the transmitter
 register (U1THR that we just wrote to) is empty.  LSR_TEMT = 0x40 = 0100 0000, so U1LSR&LSR_TEMT indicates 
 whether the U1THR register is empty (whether the byte to be sent has already been sent).  So 
 while(!transmit register is empty) will loop doing nothing until the previous byte has been sent, and the 
 transmit register is ready to send again.
 */
/*
void sendMsg_unsigned(unsigned char* s){
	//Debug("start of sendMsg_unsigned(), msg is:");
	//Debug((char*)s);
	
	unsigned char c;
	
	//wait for any previous messages to clear the port's buffer
	//otherwise we might mix up this message with on from managed code
	while (SERIAL_PORT_SENDING);
	
	//send the message char by char, incrementing s until we reach a null character (end of string)
	while(*s)
	{
		c = s[0];
		U0THR = (c);
		while (SERIAL_PORT_SENDING);
		s++;
	}		
	
	//write a newline character to signal the end of the message
	U0THR = 10;
	while (SERIAL_PORT_SENDING);
}*/

//overload to make things a little easier
#if HAS_STRING_FUNC == 1
void sendMsg(char* s){
	while(*s)
	{	
		sendByte(*s++);
	}
}
#endif

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