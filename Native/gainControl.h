#include "fourier.h"
#include "pwm.h"
#include "helpers.h"

//the max number of the pwm, but it actually counts to this value so there is 1 extra gain level
#define PWM_LEVELS		18

#define EMA_RESET_DENOM 	50.0F
#define EMA_TARGET			65536
#define EMA_RESET_NUM		(EMA_RESET_DENOM * EMA_TARGET)
#define EMA_RANGE			20000
#define EMA_WEIGHT			0.995F

//a naive way to reset the ema when the gain level is changed.  Using something more clever, like multiplying it by the ratio of 
//old to new gain levels would be better, but we'll worry about that later
#define resetEma(channel) emaNums[channel] = EMA_RESET_NUM; emaDenoms[channel] = EMA_RESET_DENOM;

//stores the current pwm gain level, out of PWM_LEVELS + 1
unsigned char pwmLevels[NUM_CHANNELS];

//the gain level of each pwm duty cycle, measured experimentallyn
//These **MUST** be less or equal to 64 - maximum value stored for reading is 16bits = 65535, max reading is 1023, 
//multiplying by a gain of > 64 would cause it to loop around and give a low reading.  BAD!!
//also, top 1 should be close to 64 to use the full range and therefore get hte most resolution
const float gainLevels[PWM_LEVELS+1] = {
#if PWM_LEVELS == 18
1.0, 
1.0, 
1.0,
1.0, 
1.0, 
1.0, 
1.0, 
1.0,
1.0, 
1.0, 
1.0, 
1.0, 
1.0,
1.0, 
1.0, 
1.0, 
1.0, 
1.0,
1.0
#else
#error Gain values not defined for the provided PWM_LEVELS
#endif
};

//ema values used to control the gain so it doesn't constantly change and go crazy
float emaNums[NUM_CHANNELS];
float emaDenoms[NUM_CHANNELS];

//Should probably use a very long term ema of points to get a better estimate of this.
//do it per channel as well	
float center = 512.0F;
	
unsigned short addGainValue(unsigned short value, unsigned char channel){
	
	//find the distance between the value and the center of the reading voltages
	float diffSquared = (value - center) * (value - center);
	
	//add contributions to the ema numerator and denominator
	emaNums[channel] = emaNums[channel] * EMA_WEIGHT + diffSquared;
	emaDenoms[channel] = emaDenoms[channel] * EMA_WEIGHT + 1.0f;
	//calculate the ema
	float ema = emaNums[channel] / emaDenoms[channel];
	
	//adjust the value using the current levels
	value = (unsigned short)(value * gainLevels[pwmLevels[channel]]);
	
	//adjust gain level in the right direction if we need to, and aren't at either limit
	//if((ema > (EMA_TARGET + EMA_RANGE)) && (pwmLevels[channel] < PWM_LEVELS))
	if(floatLT(EMA_TARGET + EMA_RANGE, ema) && (pwmLevels[channel] < PWM_LEVELS))
	{
		pwmLevels[channel]++;
		setPwm(channel, pwmLevels[channel]);
		resetEma(channel);
	//} else if ((ema < EMA_TARGET - EMA_RANGE) && (pwmLevels[channel] > 0))
	} else if (floatLT(ema, EMA_TARGET - EMA_RANGE) && (pwmLevels[channel] > 0))
	{
		pwmLevels[channel]--;
		setPwm(channel, pwmLevels[channel]);
		resetEma(channel);
	}
	
	return value;
}