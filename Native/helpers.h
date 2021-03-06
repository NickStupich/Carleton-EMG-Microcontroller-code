#ifndef HELPERS
#define HELPERS
#include "settings.h"

#if CUSTOM_FLOAT_COMPARE == 1

#define EXPONENT_BITS		0x7F800000
#define MANTISSA_BITS		0x7FFFFF
#define SIGN_BIT			0x80000000

unsigned int floatLT(float x, float y);

#else

#define floatLT(x, y)	(x < y)

#endif

#endif
