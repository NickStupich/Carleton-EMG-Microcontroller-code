#ifndef PWM
#define PWM

/*pins = bitmask for each pwm(1-6), in bits 0-5:
pins[0] = pwm1
pins[1] = pwm2
pins[2] = pwm3
pins[3] = pwm4
pins[4] = pwm5
pins[5] = pwm6
pins[6], pins[7] - unused
1 - enabled
0 - disabled
period = desired pwm cycle period in clock cycles (1/18us)
*/
void pwmSetup(char pins, unsigned int period);

/*
pin = 0-5 for pwm1-5
positiveDutyCycle = length of positive duty cycle in clock cycles(1/18us)
	this should be less/equal to the period provided to setup()
*/
void setPwm(char pin, unsigned int positiveDutyCycle);

#endif