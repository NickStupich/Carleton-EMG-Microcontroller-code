#include "Debug.h"
#include "Bluetooth.h"
#include "AnalogRead.h"
#include "fourier.h"
#include "time.h"
#include "pwm.h"
#include "scheduling.h"
#include "gainControl.h"

//about 1kHz, but this way the bins from the fft are exactly 4hz apart
#define TARGET_READ_FREQUENCY  	1024	//Hz
#define TARGET_FFT_FREQUENCY	40		//Hz

unsigned short currentData[NUM_CHANNELS][DATA_LENGTH];	//where the analog reads go

float transformData[DATA_LENGTH];	//copy of data for the channel being FFTed.  gets replaced with frequency data by fft function.

unsigned char transformBins[FOURIER_BINS];	//nice short little array of bins of interest that were combined together from the fft
unsigned char transformScalingValue;		//scaling value calculated by the binning algorithm

counter_t dataIndex = 0;	//index of buffers we're filling

void AnalogReadCallback(void *arg){
	unsigned char channel;
	unsigned short value;
	for(channel = 0;channel < NUM_CHANNELS;channel++)
	{
	
#if READ_EVERY_CHANNEL == 0
		if(CHANNEL_IS_ENABLED(status, channel))
		{
#endif
			value = analogRead(channel);	
			value = addGainValue(value, channel);
			currentData[channel][dataIndex] = value;
			
//close the brace if we're not reading every channel
#if READ_EVERY_CHANNEL == 0
		}
#endif
	}
	
	//increment the address and loop back around if neccessary
	dataIndex++;
	dataIndex %= DATA_LENGTH;
}

void sendTransformData(unsigned char scalingValue, unsigned char data[]){
	int i;
	sendByte(scalingValue);
	
	//send all the data
	for(i=0;i<FOURIER_BINS;i++)
	{
		sendByte(data[i]);
	}	
}

void copyDataToTempArray(unsigned char channel){
	//  copy data into the transform data array
	//Some notes:
	//	In the array currentData, there is an index "dataIndex" where the latest value was inserted
	//	once that index reaches the end of the array, it will go back to 0, so the array has the latest 
	//	DATA_LENGTH analog reads, with the oldest starting at index+1, going to the end of the array, and then
	//	from the start up to index
		
	//	for the array that is being transformed, we want it to start at 0.  Thats why it's NOT just a straight
	//	memcpy(transformArray, currentArray, DATA_LENGTH)
	
	
	//there is likely a better way to do this - on a quick check no implementation of memcpy available...?
	//also, should probably do some sort of concurrency lock?
	
	//as a half assed concurrency lock, take a copy of the data index in case it changes while copying data.
	//not even close to failproof...but pretty quick.  and it should at least prevent crashing due to 
	//overruning bounds of array, though there will be some improperly copied values
	
	//copy over data to an array so we don't modify it while doing the fft
	int tempDataIndex = dataIndex;
	int i,j;
	
	for(i=tempDataIndex+1,j=0;i<DATA_LENGTH;i++)
	{
		transformData[j++] = (float)currentData[channel][i];
	}
	for(i=0;i<=tempDataIndex;i++)
	{
		transformData[j++] = (float)currentData[channel][i];
	}
}
/* //better implementation that didn't seem to work, but that could very well have been an unrelated problem because it worked excellently on
//some test code.  on the other hand, not much smaller or faster so oh well, i guess the compiler is pretty clever
void copyDataToTempArray(unsigned char channel)
{
	//slightly smaller implementation that just straight up array copying
	counter_t i, tempDataIndex = dataIndex;
	
	float* to;
	unsigned short* from;
	
	//first half 
	to = transformData;
	from = currentData[channel] + tempDataIndex + 1;
	
	for(i=DATA_LENGTH - tempDataIndex - 1;i>0;i--)
	{
		*to++ = (float)*from++;
	}
	
	//second half
	from = currentData[channel];
	for(i=tempDataIndex;i>=0;i--)
	{	
		*to++ = (float)*from++;
	}
}*/

void FourierCallback(void *arg){
	unsigned char channel;
	
	//send the control byte to indicate the start of the ffts
	sendByte(DATA_MAXIMUM);
	
	//iterate over each channel, only doing stuff if we're using that channel
	for(channel = 0;channel < NUM_CHANNELS;channel++)
	{
		if(CHANNEL_IS_ENABLED(status, channel))
		{			
			copyDataToTempArray(channel);
			
			/*note: realft() is written for a array with starting index of 1 (array[1] is the first element)
			obviously c doesn't work like that, so we just subtract one from the address to make it work.
			this is specifically mentioned in the first complete paragraph on page 507 of "NUMERICAL RECIPES in C"
			http://www.mathcs.org/java/programs/FFT/FFTInfo/c12-2.pdf
			*/
			#pragma GCC diagnostic ignored "-Warray-bounds"
			realft(transformData-1, DATA_LENGTH);
			#pragma GCC diagnostic pop
	
			//bin the data to be sent
			transformToBins(transformData, DATA_LENGTH, transformBins, &transformScalingValue);
			
			
			//send away the data
			sendTransformData(transformScalingValue, transformBins);
		}
	}
}

int Start(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){	
	if(!(status & KEEP_RUNNING_MASK))
	{
		status = (char)(*(int*)args[0]);
		
		//send the start byte back as an acknowledgement
		sendByte(status);
		
		//set up variables and initialize the analog read task
		
		//analog read callback
		items[0].initialDelay = 0;
		items[0].targetFrequency = TARGET_READ_FREQUENCY;
		items[0].delay = 1000000 / TARGET_READ_FREQUENCY;
		items[0].function = &AnalogReadCallback;
		items[0].isKernelMode = RLP_TRUE;	//high priority, will interrupt the fft callback
		
		//fft callback
		//initial delay is high enough to that the analog reads can fill the buffer (thrice) to make sure we don't send an 
		//fft based on garbage data.  Currently the delay is only about 0.5 seconds
		items[1].initialDelay = 3000000 * DATA_LENGTH / TARGET_READ_FREQUENCY;
		items[1].targetFrequency = TARGET_FFT_FREQUENCY;
		items[1].delay = 0;
		items[1].function = &FourierCallback;
		items[1].isKernelMode = RLP_FALSE;
		
		setupScheduling();
	} 
	else 
	{
		//send two control bytes as a failure message
		sendByte(DATA_MAXIMUM);
		sendByte(DATA_MAXIMUM);
	}
	return status;
}

int Stop(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	status = 0;
	
	pwmSetup(63, PWM_LEVELS);	//reset the pwms back to very high duty cycle
	//this will hopefully help to avoid damaging the microcontroller if we get large voltages to the analog in when we're not
	//actively doing gain control
	return status;
}

/* This should be called at startup.
Starts the clocks for the Switched Cap filter, and initalizes the PWMs and sets their duty cycle to 50%
*/
int Init(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	//start up the 6kHz clock
	#include "timer3Clock.h"
	
	//start up the 12kHz clock
	#include "i2sClock.h"
	
	//start up the pwms (all pins).  at this point we don't know what channels will run to just start up everything
	pwmSetup(63, PWM_LEVELS);
	
	return 1;
}