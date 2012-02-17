#include "RLP.h"
#include "time.h"
#include "settings.h"

/*so...this may seem retarded at first...
HOWEVER:
	this board has very little space.  If we do integer division, the libraries _udivsi3.o and _dvmd_tls.o need to be included
	These are 114H and 4H bytes each = 280 bytes (2.7% of total space).  Currently there is 1 division / second / scheduled item and 
	1 per scheduled item for setup, as well as the casts to float and back to unsigned int.  The performance hit will be very minimal.  
	The code size overhead is also quite small, 36 bytes for both divisions that are currently being done. So this saves 244 bytes
	at the moment.  If integer divisions are later required, this should absolutely be turned off since the library will be included
	anyways, and turning it off will result in smaller, faster code.	
*/

#define CHANNEL_IS_ENABLED(status, channel)		status & (1<<channel)
#define KEEP_RUNNING_MASK						1<<7
#define NUM_ITEMS								2
#define SCHEDULING_UPDATE_INTERVAL 				ONE_SECOND

typedef struct item {
	RLP_Task task;
	unsigned int initialDelay;
	unsigned int targetFrequency;
	unsigned int delay;
	RLP_CALLBACK_FPN function;
	unsigned int isKernelMode;
	time_t currentSecondStart;
	unsigned short readsThisSecond;
	unsigned char index;
} item;

unsigned char status = 0;	
/*	whether to keep everything running, so we can stop it from managed code easily
*	also keeps track of which analog pins should be read from 
	Format:
		bit0-5 (LSB-5) - each bit indicates whether that channel should be read from 
		bit 6 - whether the reading and fft should currently be running.
		bit 7 - undefined so far
*/

item items[NUM_ITEMS];
void processCallback(void *arg){

	unsigned char id = *(unsigned int*) arg;

	//check if we have been told to stop running stuff
	if(!(status & KEEP_RUNNING_MASK)){
		RLPext->Task.Abort(&items[id].task);
		return;
	}
	
	//do the actual work we want the function to do
	items[id].function(0);
	
	//figure out when we should next call the function
	time_t curTime = readTimer();	
	
	if(curTime < items[id].currentSecondStart)	//timer overflowed(happens every ~4mins, skip adjusting this time)
	{
		items[id].currentSecondStart = readTimer();
		items[id].readsThisSecond=0;
	} 
	else if((curTime - items[id].currentSecondStart) < SCHEDULING_UPDATE_INTERVAL)	//still on the same second, just increment readsThisSecond
	{
		items[id].readsThisSecond++;
	} 
	else //new second, adjust the interval time
	{
		items[id].currentSecondStart = curTime;
#if FLOATDIV_INSTEAD_OF_UINT == 1
		items[id].delay = (unsigned int)(((float)(items[id].delay * items[id].readsThisSecond)) / (float)items[id].targetFrequency);
#else
		items[id].delay = (items[id].delay * items[id].readsThisSecond) / items[id].targetFrequency;
#endif
		
		//Debug_uint(items[id].readsThisSecond);
		//Debug_uint(items[id].delay);
		
		items[id].readsThisSecond = 0;
	}
	
	RLPext->Task.ScheduleTimeOffset(&items[id].task, items[id].delay);	
}

void setupScheduling()
{
	unsigned char i;
	for(i=0;i<NUM_ITEMS;i++)
	{
		//some stuff that needs to be set
		items[i].readsThisSecond = 0;
		items[i].index = i;
#if FLOATDIV_INSTEAD_OF_UINT == 1
		items[i].delay = (unsigned int)(1000000.0f / (float)items[i].targetFrequency);
#else
		items[i].delay = 1000000 / items[i].targetFrequency;
#endif	
		//initialize the task
		RLPext->Task.Initialize(&items[i].task, processCallback, (void*)&items[i].index, items[i].isKernelMode);
		
		items[i].currentSecondStart = readTimer();
		//schedule the task
		if(items[i].initialDelay > 0)
		{
			//we also need to correct the start time so that the counts don't get messed up due to the initial delay
			//just assume the task will run (and complete) at exactly the right time
			
			//add the initial delay * 18 since we're running at 18 cycles / us.  Overflow shouldn't be a problem (?) since 
			//the timer will just roll back around to zero like the variable
			items[i].currentSecondStart += items[i].initialDelay * 18;
			RLPext->Task.ScheduleTimeOffset(&items[i].task, items[i].initialDelay);
		}
		else
		{
			RLPext->Task.Schedule(&items[i].task);
		}		
	}	
}