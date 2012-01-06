#include <Bluetooth.h>

#include <lpc23xx.h>

#define LSR_TEMT    0x40 //1 << 7;? - test
#define SERIAL_PORT_SENDING	!(U0LSR & LSR_TEMT)

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
	U0THR = c;
}