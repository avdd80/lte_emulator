#include <stdio.h>
#include "gpu_fft.h"
#include "mailbox.h"

#define FFT_WINDOW_SIZE_LOG2N 11
#define FFT_WINDOW_SIZE       (1 << FFT_WINDOW_SIZE_LOG2N)

struct GPU_FFT_COMPLEX *data_in_ptr;
struct GPU_FFT *fft;
struct GPU_FFT_COMPLEX *fft_out;

/* Generates and saves the complex conjugate sample at the mirror 
   index of the FFT */
void generate_complex_conjugate (struct GPU_FFT_COMPLEX *ptr, unsigned int index)
{
	ptr[FFT_WINDOW_SIZE - index].re = ptr[index].re;
	ptr[FFT_WINDOW_SIZE - index].im = -1 * ptr[index].im;
}

int main ()
{
	int i, ret, mb = mbox_open ();
	
    ret = gpu_fft_prepare(mb, FFT_WINDOW_SIZE_LOG2N, GPU_FFT_REV, 1, &fft); // call once
    
    printf ("Prepare FFT done\n");
    
    data_in_ptr = fft->in;

	switch(ret) {
		case -1: printf("Unable to enable V3D. Please check your firmware is up to date.\n");         return -1;
		case -2: printf("log2_N=%d not supported.  Try between 8 and 22.\n", FFT_WINDOW_SIZE_LOG2N);  return -1;
		case -3: printf("Out of memory.  Try a smaller batch or increase GPU memory.\n");             return -1;
		case -4: printf("Unable to map Videocore peripherals into ARM memory space.\n");              return -1;
		case -5: printf("Can't open libbcm_host.\n");                                                 return -1;
    }

	for (i = 0; i < FFT_WINDOW_SIZE; i++)
	{
		data_in_ptr[i].re = 0;
		data_in_ptr[i].im = 0;
	}
	data_in_ptr[150].re = 60;
	data_in_ptr[150].im = 60;
	generate_complex_conjugate(data_in_ptr, 150);
	data_in_ptr[300].re = -60;
	data_in_ptr[300].im = 60;
	generate_complex_conjugate(data_in_ptr, 300);



	usleep(1); // Yield to OS
	gpu_fft_execute(fft); // call one or many times
	
	printf ("Execute FFT done\n");
	
	fft_out = fft->out;
	
	for (i = 0; i < FFT_WINDOW_SIZE; i++)
	{
		printf ("%f,%f\n", data_in_ptr[i].re, data_in_ptr[i].im);
	}

    gpu_fft_release(fft); // Videocore memory lost if not freed !
	return 0;
}