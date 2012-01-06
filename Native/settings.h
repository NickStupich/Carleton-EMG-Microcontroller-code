
//in scheduling, use float division instead of requiring additiona libraries to to unsigned integer division
#define FLOATDIV_INSTEAD_OF_UINT		1

//in time, whether to make the timer reading inline with a preprocessor define or make it a function call
#define makeInline						1

//debugging on or off
#define DEBUG_ON 						1

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