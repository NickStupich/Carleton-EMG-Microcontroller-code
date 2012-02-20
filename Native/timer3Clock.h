//10 kHz signal using timer3, making the clock output at pin di8
PCONP |= 1<<23;		//Timer3 power enabled - turned on above
PINSEL0 |= 3<<20;		//MAT3.0 on pins p0.10

T3TCR = 2;		//disble counter for now
T3TC = 0;		//reset timer counter
T3PC = 0;		//reset prescale counter
T3CTCR = 0;		//timer mode - every rising pclk edge
T3PR = 456;		//prescale counter counts to 456 + 1 before incrementing timer
//this should be 449 in theory to produce exactly 10kHz, but imperfectly in the switched cap mean we set the clock off by a little
//so have the notch perfectly centered at 60Hz

T3MR0 = 1;		//flip bit every timer increment
T3MCR = 2;		//reset on match with MR0
T3TCR = 1;		//enable the counter

T3CCR = 0;		//clear capture register
T3EMR = 1 | (3 << 4);	//external match 0, toggle match bit/output