
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
