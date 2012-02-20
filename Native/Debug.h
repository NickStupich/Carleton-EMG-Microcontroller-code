#ifndef DEBUG
#define DEBUG

#include "settings.h"

#if DEBUG_ON == 1

//preprocessor defines call the internal functions
#define Debug(s) 		_Debug(s);
#define Debug_uint(n) 	_Debug_uint(n);

void _Debug(char* s);
void _Debug_uint(unsigned int n);

#else

//empty preprocessor defines so nothing actually shows up
#define Debug(s)
#define Debug_uint(n)

#endif

#endif