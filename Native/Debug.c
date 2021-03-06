#include "Debug.h"
#include "lpc23xx.h"
#include "settings.h"

#define UINT_STRING_LENGTH	10
#define LSR_TEMT    (1<<7)
#define SERIAL_PORT_SENDING_COM2	!(U1LSR & LSR_TEMT)

#if DEBUG_ON == 1

 void _Debug(char* s){
	while(*s)
	{
		while (SERIAL_PORT_SENDING_COM2);
		U1THR = *s++;
	}
	
	while (SERIAL_PORT_SENDING_COM2);
	U1THR = 10;
 }
 
 void _Debug_uint(unsigned int n){
	char s[UINT_STRING_LENGTH];
	int i=0, j=0;
	char c;
	
	//put in a string in reverse order
	do {       // generate digits in reverse order 
	 s[i++] = n % 10 + '0';   // get next digit 
     } while ((n /= 10) > 0);     // delete it 
     s[i] = '\0';
	 
	 //reverse string
	 for(j=0,i--;j<i;j++,i--)
	 {
		c = s[j];
        s[j] = s[i];
        s[i] = c;
	 }
	
	_Debug(s);
 }
 #endif
 