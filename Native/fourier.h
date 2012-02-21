#ifndef FOURIER
#define FOURIER

//number of data values stored and used for the fft
#define DATA_LENGTH				256
#define counter_t				unsigned char

//number of bins that the fft is stuck into before being sent away 
#define FOURIER_BINS			8

//maximum data value is 1 less...this is probbaly kinda a dumb way to do it
#define DATA_MAXIMUM			255

#define MAX_TO_SEND_FLOAT		254.0f

#define MAX_SQRT_ERROR	0.1f

//number of individual frequencies that are stuck together into 1 bin before being sent off 
#define FREQUENCIES_PER_BIN		3

#define START_BIN				3

#define SIXTY_HZ_BIN			15

#define LAST_BIN				(START_BIN + FREQUENCIES_PER_BIN * FOURIER_BINS + 1)

void realft(float data[], unsigned long n);// unsigned char transformBins[], unsigned char *transformScalingValue);

void transformToBins(float data[], unsigned long n, unsigned char transformBins[], unsigned char *transformScalingValue);


#endif