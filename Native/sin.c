#include "sin.h"

const float data[9] = {
-1,
1.22464679915e-16,
1.0,
0.707106781187,
0.382683432365,
0.195090322016,
0.0980171403296,
0.0490676743274,
0.0245412285229
};

float sin(unsigned int x)
{
	unsigned short n;
	for(n=8; (1<<n) > x;n--) {}
	return data[n];
}