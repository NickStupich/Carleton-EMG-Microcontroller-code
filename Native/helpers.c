#include "helpers.h"

#if CUSTOM_FLOAT_COMPARE == 1

/*works only when both numbers are >0*/
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
