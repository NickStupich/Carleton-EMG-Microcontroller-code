#ifndef SETTINGS
#define SETTINGS


//in scheduling, use float division instead of requiring additiona libraries to do unsigned integer division
/*so...this may seem retarded at first...
HOWEVER:
	this board has very little space.  If we do integer division, the libraries _udivsi3.o and _dvmd_tls.o need to be included
	These are 114H and 4H bytes each = 280 bytes (2.7% of total space).  Currently there is 1 division / second / scheduled item and 
	1 per scheduled item for setup, as well as the casts to float and back to unsigned int.  The performance hit will be very minimal.  
	The code size overhead is also quite small, 36 bytes for both divisions that are currently being done. So this saves 244 bytes
	at the moment.  If integer divisions are later required, this should absolutely be turned off since the library will be included
	anyways, and turning it off will result in smaller, faster code.	
*/
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
//***Now that gain control is running (unless it's turned off), this shoudn't be set to 1.  Gain control is pretty 
//computationally expensive so we shouldn't be doing it unneccessarily on all channels
#define READ_EVERY_CHANNEL				0

//whether to turn the gain control circuit on or off.  If off, pwms will sit at 50% duty cycle, and there will be no scaling of 
//analog reads
#define GAIN_CONTROL_ON					1

//number of channels that can be read from (number that actually run is determined by external UI in start byte, this is just the max.
//  Maximum is 6
#define NUM_CHANNELS					6

//the max number of the pwm, but it actually counts to this value so there is 1 extra gain level
#define PWM_LEVELS		18

#endif