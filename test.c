#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "gpu_fft.h"
#include "mailbox.h"

#define FFT_WINDOW_SIZE_LOG2N 11
#define FFT_WINDOW_SIZE       (1 << FFT_WINDOW_SIZE_LOG2N)
GPU_FFT_COMPLEX data_in[FFT_WINDOW_SIZE];

struct GPU_FFT *fft;
struct GPU_FFT_COMPLEX *fft_out;

int main ()
{
	int i, mb = mbox_open();
	for (i = 0; i < 2048; i++)
	{
		data_in[i].re = 0;
		data_in[i].im = 0;
	}
	
	data_[256].re = 2048;
	
	mb = mbox_open (void);
	
    ret = gpu_fft_prepare(mb, FFT_WINDOW_SIZE_LOG2N, GPU_FFT_REV, 1, &fft); // call once

	switch(ret) {
		case -1: printf("Unable to enable V3D. Please check your firmware is up to date.\n");         return -1;
		case -2: printf("log2_N=%d not supported.  Try between 8 and 22.\n", FFT_WINDOW_SIZE_LOG2N);  return -1;
		case -3: printf("Out of memory.  Try a smaller batch or increase GPU memory.\n");             return -1;
		case -4: printf("Unable to map Videocore peripherals into ARM memory space.\n");              return -1;
		case -5: printf("Can't open libbcm_host.\n");                                                 return -1;
    }


	usleep(1); // Yield to OS
	gpu_fft_execute(fft); // call one or many times
	
	fft_out = fft->out;
	
	for (i = 0; i < FFT_WINDOW_SIZE; i++)
	{
		printf ("%d ", fft_out[i]);
	}

    gpu_fft_release(fft); // Videocore memory lost if not freed !
	return 0;
}