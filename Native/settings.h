#ifndef SETTINGS
#define SETTINGS

//in scheduling, use float division instead of requiring additiona libraries to do unsigned integer division
#define FLOATDIV_INSTEAD_OF_UINT		1

//in time, whether to make the timer reading inline with a preprocessor define or make it a function call
#define makeInline						1

//debugging on or off
#define DEBUG_ON 						0

//whether we can send strings over bluetooth
#define HAS_STRING_FUNC					0

//whether to use the custom less than comparison for floats
#define CUSTOM_FLOAT_COMPARE			1	

//instead of actual analog value readings just create a sawtooth wave
#define DOSAWTOOTH						0

//whether to read into every channel, or only those that we care about
//turning on saves 16 bytes, and takes a constant 88cycles for reads, while 
//turning it off takes ~35 + 10 * #channels for reads
//almost no difference either way
#define READ_EVERY_CHANNEL				1

//whether to turn the gain control circuit on or off.  If off, pwms will sit at 50% duty cycle, and there will be no scaling of 
//analog reads
#define GAIN_CONTROL_ON					0

//number of channels that can be read from.  Maximum is 6
#define NUM_CHANNELS					6

//the max number of the pwm, but it actually counts to this value so there is 1 extra gain level
#define PWM_LEVELS		18

#endif