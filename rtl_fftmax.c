// derived from http://www.m0dts.co.uk/files/simple_rtlsdr_fft.c
//	http://www.m0dts.co.uk/
// to compile: gcc rtl_fttmax.c -o rtl_fftmax -lfftw3 -lm
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>

#define FREQ	0L
#define	SPS	1000000L	/* freq. is labelled in MHz */
#define	NUM_POINTS	512

unsigned char	*iq_buf;
double	*binp; 	/* bin table pointer (mag squared) */
double	*max_binp; /* biggest value for that bin */
/*
** input : raw rtl_sdr IQ file as STDIN
** output 2 columns table : frequency/20log(fft bins)
** optional args : frequency, sample rate, #bins
*/
int main(int argc, char *argv[])
	{
	int	freq, sps, num_points;
	int	opt;
	int	i;
	unsigned char	*ucp;
	fftw_plan my_plan;
	fftw_complex *in, *out;
	double mr,mi;

	freq = FREQ;
	sps = SPS;
	num_points = NUM_POINTS;
	while ((opt = getopt(argc, argv, "f:s:n:")) != -1)
		{
		switch (opt)
			{
			case 'f':
				freq = atoi(optarg);
				break;
			case 's':
				sps = atoi(optarg);
				break;
			case 'n':
				num_points = atoi(optarg);
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-f frequency] [-s samples-per-sec] [-n num-points] < iq-file > ft-table\n", argv[0]);
				fprintf(stderr, "opt=0x%x\n", opt);
				exit(-1);
			}
		}
	fprintf(stderr, "freq=%d; sps=%d num_points=%d\n", freq, sps, num_points);
	if ((iq_buf=malloc(2*num_points)) == NULL)
		{
		perror("malloc(iq_buf)");
		exit(-1);
		}
	if ((binp=malloc(sizeof(binp[0])*num_points)) == NULL)
		{
		perror("malloc(bins)");
		exit(-1);
		}
	if ((max_binp=malloc(sizeof(max_binp[0])*num_points)) == NULL)
		{
		perror("malloc(max_bins)");
		exit(-1);
		}
	for (i=0; i < num_points; i++)
		max_binp[i] = 0;

	//Configure FFTW to convert the samples in time domain to frequency domain

	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*num_points);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*num_points);
	my_plan = fftw_plan_dft_1d(num_points, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	while (read(0, iq_buf, 2*num_points) == (2*num_points))
		{
		//convert buffer from IQ to complex ready for FFTW, rtlsdr outputs IQ data as IQIQIQIQIQIQ
		for (i = 0, ucp = iq_buf; i < num_points; i++)
			{
			__real__  in[i] = *ucp++ - 127;	//sample is 127 for zero signal,so 127 +/-127
			__imag__ in[i]  = *ucp++ - 127;
			}
		fftw_execute(my_plan);
		for (i = 0; i < num_points; i++)
			{
			mr = __real__ out[i];
			mi = __imag__ out[i];
			binp[i] = mr*mr + mi*mi;	/* squared magnitude */
			if (binp[i] > max_binp[i])
				max_binp[i] = binp[i];
			}
		}
#define XX(x)	(20 * log10(sqrt(x)))	/* dB */
	for (i = num_points/2; i < num_points; i++)
		{
		printf("%12.9f %9.5f\n", ((double)freq - sps + i*((double)sps/num_points))/1000000, XX(max_binp[i]));
		}
	for (i = 0; i < num_points/2; i++)
		{
		printf("%12.9f %9.5f\n", ((double)freq + i*((double)sps/num_points))/1000000, XX(max_binp[i]));
		}
	exit(0);
	}
