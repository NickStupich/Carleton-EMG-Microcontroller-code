#include "fourier.h"
#include "sin.h"
#include "Debug.h"
#include "settings.h"
#include "helpers.h"

/*
514 Chapter 12. Fast Fourier Transform
visit website http://www.nr.com or call 1-800-872-7423 (North America only),
or send email to trade@cup.cam.ac.uk (outside North America).
readable files (including this one) to any server
computer, is strictly prohibited. To order Numerical Recipes books,
diskettes, or CDROMs
Permission is granted for internet users to make one paper copy for their own personal use. Further reproduction, or any copying of machineCopyright (C) 1988-1992 by Cambridge University Press.
Programs Copyright (C) 1988-1992 by Numerical Recipes Software. 
Sample page from NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
*/

void four1(float data[], unsigned long nn)
/*Replaces data[1..2*nn] by its discrete Fourier transform, if isign is input as 1; or replaces
*data[1..2*nn] by nn times its inverse discrete Fourier transform, if isign is input as -1.
*data is a complex array of length nn or, equivalently, a real array of length 2*nn. nn MUST
*be an integer power of 2 (this is not checked for!).
*/
{
	unsigned long n,mmax,m,j,istep,i;
	float wtemp,wr,wpr,wpi,wi;
	float tempr,tempi;
	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) { //This is the bit-reversal section of the routine.
		if (j > i) { 
			//if(j+1 >= PRINTNUM)
			//	Debug_uint(j+1);
			//if(i+1 >= PRINTNUM)
			//	Debug_uint(i+1);
			SWAP(data[j],data[i]); //Exchange the two complex numbers.
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}

	//Here begins the Danielson-Lanczos section of the routine.
	mmax=2;
	while (n > mmax) { //Outer loop executed log2 nn times.

		istep=mmax << 1;
		//theta=TWO_PI/mmax; //Initialize the trigonometric recurrence.
		//Debug_uint(mmax)
		//Debug_uint(mmax * 2)
		wtemp=sin(mmax << 1);
		wpr = -2.0f*wtemp*wtemp;

		wpi=sin(mmax);
		wr=1.0f;
		wi=0.0f;
		for (m=1;m<mmax;m+=2) { //Here are the two nested inner loops.
			for (i=m;i<=n;i+=istep) {
				//if(j+1 >= PRINTNUM)
				//	Debug_uint(j+1);
				//if(i+1 >= PRINTNUM)
				//	Debug_uint(i+1);
					
				j=i+mmax; //This is the Danielson-Lanczos formula:
				tempr=wr*data[j]-wi*data[j+1]; 
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
				
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr; //Trigonometric recurrence.
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}
/*
some neato hack for sqrt based on the representation of floats
originally in quake 3:
	http://www.codemaestro.com/reviews/9

original done on x86 but they both have the same float representation:
http://www.peter-cockerell.net/aalp/html/app-b.html

more explanation:
http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Approximations_that_depend_on_IEEE_representation
*/
float _sqrt(float z)
{
	union
    {
        int tmp;
        float f;
    } u;
 
    u.f = z;
	
    u.tmp -= 1 << 23; // Subtract 2^m. 
    u.tmp >>= 1; // Divide by 2. 
    u.tmp += 1 << 29; // Add ((b + 1) / 2) * 2^m. 
	
	u.f = 0.5f * (u.f + z / u.f);	//one iteration of newton's method for a more precise answer
    return u.f;
}


void realft(float data[], unsigned long n)//, unsigned char transformBins[], unsigned char *transformScalingValue)
/*Calculates the Fourier transform of a set of n real-valued data points. Replaces this data (which
*is stored in array data[1..n]) by the positive frequency half of its complex Fourier transform.
*The real-valued First and last components of the complex transform are returned as elements
*data[1] and data[2], respectively. n must be a power of 2. This routine also calculates the
*inverse transform of a complex data array if it is the transform of real data. (Result in this case
*must be multiplied by 2/n.)
*/
{
	//Debug("RF");
	unsigned long i,i1,i2,i3,i4,np3;
	float c1=0.5f,c2,h1r,h1i,h2r,h2i;
	float wr,wi,wpr,wpi,wtemp;//,theta;// Double precision for the trigonomet-ric recurrences.
	
	c2 = -0.5;
	four1(data,n>>1); //The forward transform is here.
	//Debug("_F1");
	
	wtemp=sin(n<<1);
	wpr = -2.0f*wtemp*wtemp;
	wpi=sin(n);
	wr=1.0f+wpr;
	wi=wpi;
	np3=n+3;

	for (i=2;i<=(n>>2);i++) { //Case i=1 done separately below.
		i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
		h1r=c1*(data[i1]+data[i3]); //The two separate transforms are sep-
		h1i=c1*(data[i2]-data[i4]); //arated out of data.
		h2r = -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=h1r+wr*h2r-wi*h2i; //Here they are recombined to form the true transform of the original real data.
		data[i2]=h1i+wr*h2i+wi*h2r;
		data[i3]=h1r-wr*h2r+wi*h2i;
		data[i4] = -h1i+wr*h2i+wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr; //The recurrence.
		wi=wi*wpr+wtemp*wpi+wi;
	}
	
	data[1] = (h1r=data[1])+data[2]; //Squeeze the First and last data together to get them all within the original array.
	data[2] = h1r-data[2];
	//Debug("_RF");
}

void transformToBins(float data[], unsigned long n, unsigned char transformBins[], unsigned char *transformScalingValue)
{
	
	/*scale data
	* data is stored as [real, complex, real, complex...]
	* we only really care about the absolute value of each (real, complex) pair.
	* all other values are scaled to be within 0-256
	*/
	
	counter_t i, j;
	//Debug("FFT points");
	//combine real and imaginary parts, and put into first half of array
	
	//sqrt() is really slow, so it's importatnt to only call it on bins that we're gonna use
	//right now the 60hz bin is wasted, but i can live with that. BUT NOT MORE!
	for(i=START_BIN;i<(LAST_BIN>>1);i++)
	{
		data[i] = data[i*2] * data[i*2] + data[(i*2)+1] * data[(i*2)+1];
		data[i] = _sqrt(data[i]);
		
		//if(i<30)
			//Debug_uint((unsigned int)data[i]);
	}
	
	//combine data together into the bins that will be sent away
	float max = 0;
	j = START_BIN;
	
	//Debug("Bins");
	for(i=0;i<FOURIER_BINS;i++, j += FREQUENCIES_PER_BIN)
	{
		if(j == SIXTY_HZ_BIN) j++;	//skip over the 60hz bin
		
		data[i] = data[j] + data[j+1] + data[j+2];
		
		//Debug_uint((unsigned int) data[i]);
		//if(data[i] > max)	
		if(floatLT(max, data[i]))
			max = data[i];
	}
	
	//scale maximum value so that when each value is divided by it, the maximum attainable value will be DATA_MAXIMUM
	max /= MAX_TO_SEND_FLOAT;
	
	
	//if(max > MAX_TO_SEND_FLOAT)
	if(floatLT(MAX_TO_SEND_FLOAT, max))
	{
		max = MAX_TO_SEND_FLOAT;
	}
	else if(floatLT(max, 1.0f))
	{
		max = 1.0f;
	}
	
	//Debug("Max:");
	//Debug_uint((unsigned char) max);
	
	//stick data into the bins
	float x;
	for(i=0;i<FOURIER_BINS;i++)
	{
		//artificially limit values to < DATA_MAXIMUM
		x = data[i] / max;
		if(floatLT(MAX_TO_SEND_FLOAT, x))
			x = MAX_TO_SEND_FLOAT;
			
		transformBins[i] = (unsigned char)x;
	}
	
	//stick the maximum value back to be sent out 
	*transformScalingValue = (unsigned char)max;
}
