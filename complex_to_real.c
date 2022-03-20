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

	while (fread(ibuf, sizeof(ibuf), 1, stdin)>0) {	
		for (n=0; n<BLEN; n++) {
			if (n%2==0) {
				ibuf[n].i=-ibuf[n].i;
				ibuf[n].q=-ibuf[n].q;
			}
		}
		fwrite(ibuf, sizeof(ibuf), 1, stdout);
	}

	return 0;
}

