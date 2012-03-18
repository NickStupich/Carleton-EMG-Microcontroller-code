#include "helpers.h"

#if CUSTOM_FLOAT_COMPARE == 1

/* figure out whether x < y without needing the library with float compare() function defined in it.
returns 1 or 0 (true or false)
*/
unsigned int floatLT(float x, float y){
	typedef union data{
		float f;
		unsigned int i;
	} data;
	
	data d;
	d.f = x-y;
	return (d.i & SIGN_BIT)>>31;
}

#endif
