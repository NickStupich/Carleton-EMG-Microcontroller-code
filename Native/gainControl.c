#include "gainControl.h"
#include "pwm.h"
#include "helpers.h"
#include "settings.h"

#if GAIN_CONTROL_ON == 1

#define EMA_RESET_DENOM 	50.0F
#define EMA_TARGET			65536
#define EMA_RESET_NUM		(EMA_RESET_DENOM * EMA_TARGET)
#define EMA_RANGE			20000
#define EMA_WEIGHT			0.995F

//a naive way to reset the ema when the gain level is changed.  Using something more clever, like multiplying it by the ratio of 
//old to new gain levels would be better, but we'll worry about that later
#define resetEma(channel) emaNums[channel] = EMA_RESET_NUM; emaDenoms[channel] = EMA_RESET_DENOM;


//stores the current pwm gain level, out of PWM_LEVELS + 1
unsigned char pwmLevels[NUM_CHANNELS] = {
#if NUM_CHANNELS == 1
PWM_LEVELS/2
#elif NUM_CHANNELS == 6
PWM_LEVELS/2,
PWM_LEVELS/2,
PWM_LEVELS/2,
PWM_LEVELS/2,
PWM_LEVELS/2,
PWM_LEVELS/2
#endif
};

//the gain level of each pwm duty cycle, measured experimentally
//These **MUST** be less or equal to 64 - maximum value stored for reading is 16bits = 65535, max reading is 1023, 
//multiplying by a gain of > 64 would cause it to loop around and give a low reading.  BAD!!
//also, top 1 should be close to 64 to use the full range and therefore get hte most resolution
const float gainLevels[PWM_LEVELS+1] = {
#if PWM_LEVELS == 18
// based on values from "VCA analysis 39k" excel file
6.358299509f,
6.684840084f,
7.032369997f,
7.418016093f,
7.800663887f,
8.318243481f,
8.880388838f,
9.53608368f,
10.23249874f,
11.09613298f,
12.13505058f,
13.26914745f,
14.76743698f,
16.78469557f,
19.67356729f,
23.79147778f,
30.18183053f,
41.90959292f,
64.0f

#else
#error Gain values not defined for the provided PWM_LEVELS
#endif
};

//ema values used to control the gain so it doesn't constantly change and go crazy
#if NUM_CHANNELS == 6
float emaNums[NUM_CHANNELS] = {
	EMA_RESET_NUM, 
	EMA_RESET_NUM, 
	EMA_RESET_NUM, 
	EMA_RESET_NUM, 
	EMA_RESET_NUM, 
	EMA_RESET_NUM };
	
float emaDenoms[NUM_CHANNELS] = {
	EMA_RESET_DENOM,
	EMA_RESET_DENOM,
	EMA_RESET_DENOM,
	EMA_RESET_DENOM,
	EMA_RESET_DENOM,
	EMA_RESET_DENOM};
	
#elif NUM_CHANNELS == 1
float emaNums[NUM_CHANNELS] = {
	EMA_RESET_NUM,};
	
float emaDenoms[NUM_CHANNELS] = {
	EMA_RESET_DENOM};

#endif

//Should probably use a very long term ema of points to get a better estimate of this.
//do it per channel as well	
const float center = 512.0F;
	
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
	if(floatLT(EMA_TARGET + EMA_RANGE, ema) && (pwmLevels[channel] < PWM_LEVELS))
	{
		pwmLevels[channel]++;
		setPwm(channel, pwmLevels[channel]);
		resetEma(channel);
	} else if (floatLT(ema, EMA_TARGET - EMA_RANGE) && (pwmLevels[channel] > 0))
	{
		pwmLevels[channel]--;
		setPwm(channel, pwmLevels[channel]);
		resetEma(channel);
	}
	
	return value;
}

#endif //end of if gain control on