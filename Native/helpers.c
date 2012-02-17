#include "helpers.h"

#if CUSTOM_FLOAT_COMPARE == 1

/*works only when both numbers are >0*/
unsigned int floatLT(float x, float y){
	typedef union data{
		float f;
		unsigned int i;
	} data;
		
	data d1, d2;
	d1.f = x;
	d2.f = y;
	
	if((d1.i & EXPONENT_BITS) < (d2.i & EXPONENT_BITS))
	{
		return RLP_TRUE;
	}
	else if(((d1.i & EXPONENT_BITS) == (d2.i & EXPONENT_BITS))
		&& ((d1.i & MANTISSA_BITS) < (d2.i & MANTISSA_BITS)))
	{
		return RLP_TRUE;
	}
	
	return RLP_FALSE;
}

#endif
