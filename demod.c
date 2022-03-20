#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SRATE 2000000.0

#define BLEN (500000)

#define P_FILTER (1e-2)

typedef struct {
	int16_t i;
	int16_t q;
} sample_t;

int main(int argc, char *argv[])
{
	int n=0;

	
	sample_t ibuf[BLEN];

	if (argc<2) {
		fprintf(stderr, "Usage: %s <centre frequency, may be negative\n", argv[0]);
		return 0;
	}

	double phi=0;
	double frq=atof(argv[1]);//-52.25e+3;
	double t=0;

	double phase_error=0;
	double ferror=0;
	double a=0;
	double b=-5;

	while (fread(ibuf, sizeof(ibuf), 1, stdin)>0) {	
		double obuf[BLEN];
		for (n=0; n<BLEN; n++) {
			double i=ibuf[n].i/32768.0;
			double q=ibuf[n].q/32768.0;

			phi=phi+frq*M_PI*2/SRATE;
			double i_=cos(phi)*i-sin(phi)*q;
			double q_=sin(phi)*i+cos(phi)*q;
			
			phase_error=phase_error*(1-P_FILTER)+q_*P_FILTER;
			frq=frq+phase_error*a;
			phi=phi+phase_error*b;
			obuf[n]=i_;	
		}
		fwrite(obuf, sizeof(obuf), 1, stdout);
	}

	return 0;
}

