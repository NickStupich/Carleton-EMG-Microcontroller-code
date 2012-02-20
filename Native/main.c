#include "Debug.h"
#include "Bluetooth.h"
#include "AnalogRead.h"
#include "fourier.h"
#include "constants.h"
#include "time.h"
#include "pwm.h"
#include "scheduling.h"
#include "gainControl.h"

//about 1kHz, but this way the bins from the fft are exactly 4hz apart
#define TARGET_READ_FREQUENCY  	1024	//Hz
#define TARGET_FFT_FREQUENCY	20		//Hz

unsigned short currentData[NUM_CHANNELS][DATA_LENGTH];	//where the analog reads go

float transformData[DATA_LENGTH];

unsigned char transformBins[FOURIER_BINS];
unsigned char transformScalingValue;

counter_t dataIndex = 0;	//index of buffers we're filling

void AnalogReadCallback(void *arg){
	//sendByte(3);
	Debug("A");
	unsigned char channel;
	unsigned char value;
	//time_t start = readTimer();
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
	
	dataIndex++;
	dataIndex %= DATA_LENGTH;
	
	//Debug_uint(readTimer() - start);
}

void sendTransformData(unsigned char scalingValue, unsigned char data[]){
	//Debug("in sendTransformData()");
	int i;
	//send data on data channel serial com
	//send scaling value
	//Debug("sending");
	sendByte(scalingValue);
	//Debug_uint(scalingValue);
	
	//send all the data
	for(i=0;i<FOURIER_BINS;i++)
	{
		sendByte(data[i]);
		//Debug_uint(data[i]);
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
	//Debug("Start");
	//int t0 = readTimer();
	
	
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
/*
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
	//sendByte(4);
	Debug("F");
	
	unsigned char channel;
	
	//send the control byte to indicate the start of the ffts
	sendByte(DATA_MAXIMUM);
	
	//iterate over each channel, only doing stuff if we're using that channel
	for(channel = 0;channel < NUM_CHANNELS;channel++)
	{
		if(CHANNEL_IS_ENABLED(status, channel))
		{			
			//time_t t0 = readTimer();
			copyDataToTempArray(channel);
			//Debug("FC");
			
			//time_t t1 = readTimer();
			//do the fft
			
			/*note: realft() is written for a array with starting index of 1 (array[1] is the first element)
			obviously c doesn't work like that, so we just subtract one from the address to make it work.
			this is specifically mentioned in the first complete paragraph on page 507 of "NUMERICAL RECIPES in C"
			http://www.mathcs.org/java/programs/FFT/FFTInfo/c12-2.pdf
			*/
			#pragma GCC diagnostic ignored "-Warray-bounds"
			realft(transformData-1, 128);//DATA_LENGTH);
			#pragma GCC diagnostic pop
			
			//Debug("FF");
			//time_t t2 = readTimer();
			//bin the data to be sent
			transformToBins(transformData, DATA_LENGTH, transformBins, &transformScalingValue);
			//Debug("FB");
			//time_t t3 = readTimer();
			//Debug("after realft()");
			//send away the data
			sendTransformData(transformScalingValue, transformBins);
			//Debug("FD");
			//time_t t4 = readTimer();
			
			//Debug("copy to temp:");
			//Debug_uint(t1 - t0);
			
			//Debug("realft");
			//Debug_uint(t2 - t1);
			
			//Debug("to bins");
			//Debug_uint(t3 - t2);
			
			//Debug("send");
			//Debug_uint(t4 - t3);
		}
	}
	
	//Debug("Fe");
}

int Start(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){	
	//Debug("Start()");
	
	if(!(status & KEEP_RUNNING_MASK))
	{
		status = (char)(*(int*)args[0]);
		//Debug("S:");
		//Debug_uint(status);
		
		//send the start byte back as an acknowledgement
		sendByte(status);
		
		//set up variables and initialize the analog read task
		
		//analog read callback
		items[0].initialDelay = 0;
		items[0].targetFrequency = TARGET_READ_FREQUENCY;
		items[0].delay = 1000000 / TARGET_READ_FREQUENCY;
		items[0].function = &AnalogReadCallback;
		items[0].isKernelMode = RLP_TRUE;
		
		//fft callback
		//initial delay is high enough to that the analog reads can fill the buffer (twice) to make sure we don't send an 
		//fft based on garbage data.  Currently the delay is only about 0.5 seconds
		items[1].initialDelay = 2000000 * DATA_LENGTH / TARGET_READ_FREQUENCY;
		items[1].targetFrequency = TARGET_FFT_FREQUENCY;
		items[1].delay = 0;
		items[1].function = &FourierCallback;
		items[1].isKernelMode = RLP_FALSE;
		
		//#include "setupScheduling.h"
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
	//sendByte(8);
	return status;
}

/* This should be called at startup.
Starts the clocks for the Switched Cap filter, and initalizes the PWMs and sets their duty cycle to 50%
*/
int Init(unsigned int *generalArray, void **args, unsigned int argsCount, unsigned int *argSize){
	//start the clock for the SC notch filter (10khz and 20khz signals)
	//PCONP |= (1<<23) | (1<<27);	//power for Timer3 and I2S
	#include "timer3Clock.h"
	#include "i2sClock.h"
	//start up the pwms (all pins) with 50% duty cycle
	pwmSetup(63, PWM_LEVELS);
	
	return 1;
}