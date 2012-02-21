#include <pwm.h>
#include <lpc23xx.h>

#define PWM_COUNT 	6
#define PWM1_ADDR	0xE001801C

unsigned int _period = 18;

void pwmSetup(char pins, unsigned int period){
	_period = period;
	PWM1TCR |= 1;	//timer runs 
	
	unsigned char i;
	for(i=0;i<PWM_COUNT;i++)
	{
		if(pins & 1<<i)
		{
			PWM1PCR |= 1<<(i+9);	//pwm enabled
			PWM1PCR &= ~(2<<i);		//single edge control mode
			PWM1LER |= 1<<i;		//latch enable register enabled - will take new values
			setPwm(i, period-1);	//set initial duty cycle to almost 100%.  Having it not 100% makes it easier to make sure it's working,
				//but setting it high means that we'll have very low gain.  This will hopefully help to produce the ADCs from large voltage spikes
			
			//the pin select stuff, different pwms use either PINSEL3 or PINSEl4
			switch(i)
			{
				case 0:
					PINSEL3 |= 1<<5;
					break;
				
				case 1:
					PINSEL3 |= 1<<9;
					break;
			
				case 2:
					PINSEL3 |= 1<<11;
					break;
		
				case 3:
					PINSEL4 |= 1<<6;
					break;
				
				case 4:
					PINSEL4 |= 1<<8;
					break;
					
				case 5:
					PINSEL4 |= 1<<10;
					break;
			}
		}
	}
	
	PWM1MR0 = period-1;
}

void setPwm(char pin, unsigned int positiveDutyCycle){
	//normally subtracting 1 from duty cycle makes things like up nicely
	//however, if 0 or the period is provided, they seem to flip (0 gives the result
	//that would be expected from the period, and the period produces the result expected
	//from providing 0.  So this fixes that...
	
	if(positiveDutyCycle == 0)
		positiveDutyCycle = _period - 1;
	else if(positiveDutyCycle == _period)
		positiveDutyCycle = -1;
	else
		positiveDutyCycle-=1;
	
	switch(pin)
	{
		case 0:
			PWM1MR1 = positiveDutyCycle;
			break;
		case 1:
			PWM1MR2 = positiveDutyCycle;
			break;
		case 2:
			PWM1MR3 = positiveDutyCycle;
			break;
		case 3:
			PWM1MR4 = positiveDutyCycle;
			break;
		case 4:
			PWM1MR5 = positiveDutyCycle;
			break;
		case 5:
			PWM1MR6 = positiveDutyCycle;
			break;
	}
	/*
		PWM1MR4, 5, 6 are not together in memory with 1, 2, 3, 
		so we have 2 separate cases depending on which it is
		instead of having to subtract 3 from pin, we just add 
		pin to PWM1CR2 (3 less than PWM1MR4)
	*/
	/*
	if(pin < 3)
	{
		*((&PWM1MR1) + pin) = positiveDutyCycle-1;
	} else
	{
		*((&PWM1CR2) + pin) = positiveDutyCycle-1;
	}*/
	}