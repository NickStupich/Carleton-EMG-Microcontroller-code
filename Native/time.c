#include "time.h"

#if makeInline != 1
time_t readTimer()
{
	return (unsigned int)T0TC;
}

#endif