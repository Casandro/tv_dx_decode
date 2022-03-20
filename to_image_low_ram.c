#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define SRATE 2000000

#define FWIDTH (64*SRATE/1000000)
#define FLINES (312.5)

#define FSIZE (FWIDTH*313)

#define FBSIZE (FWIDTH*1250)

double image[FBSIZE];

#define OLINES (625)
#define OWIDTH (FWIDTH*2)

#define TIMGSIZE (OLINES*256)

int main(int argc, char *argv[])
{
/*	if (argc<2) {
		printf("Usage: %s <infile>\n infile has decoded values as double floats\n", argv[0]);
		return 1;
	}*/


	fread(image, 1, sizeof(image), stdin);

	int n=0;
	double t=0;
	double i=0;
	
	double avg_f=1;
	int frame=0;
	while (!feof(stdin)){
		int line;

		double fimg[TIMGSIZE];
		for (line=0; line<OLINES; line++) {
			int lpos=FWIDTH*line;
			int px=0;
			for (px=0; px<OWIDTH; px++) {
				int ppos=lpos+px;
				fimg[line*OWIDTH+px]=(image[lpos+px]*avg_f)+(fimg[line*OWIDTH+px]*(1-avg_f));
			}
		}

		double max=0;
		double min=99999;
		int px=0;
		for (px=4; px<OWIDTH-4; px++) {
			double sum=0;
			int cnt=0;
			for (line=0; line<OLINES; line++) {
				int x=0;
				for (x=-4;x<=4; x++){ 
					sum=sum+fimg[line*OWIDTH+px+x];
					cnt=cnt+1;
				}
			}
			sum=sum/cnt;
			if (sum>max) max=sum;
			if (sum<min) min=sum;
		}

		printf("Frame %d; min %lf max %lf\n", frame, min, max);

		uint8_t iimg[TIMGSIZE];
		for (line=0; line<625; line++) {
			int px=0;
			for (px=0; px<OWIDTH; px++) {
				double fi=fimg[line*256+px];
				double d=((fi)/(max))*0.5+0.25;
				int v=(256-(d)*256);
				if (v<0) v=0;
				if (v>255) v=255;
				iimg[line*256+px]=v;
			}
		}
		char fname[128];
		memset(fname, 0, sizeof(fname));
		snprintf(fname, sizeof(fname)-1, "IMG-%08d.pgm", frame);
		FILE *fi=fopen(fname, "w");	
		memset(fname, 0, sizeof(fname));
		snprintf(fname, sizeof(fname)-1, "P5 %d %d 255\n",OWIDTH, OLINES);
		fwrite(fname, strlen(fname), 1, fi);
		fwrite(iimg, sizeof(iimg), 1, fi);
		fclose(fi);

		int lineno=312+(frame%2);
		size_t flen=sizeof(double)*(lineno*FWIDTH);
		void *new_start=((void *)image)+flen;
		void *new_end=((void*)image)+sizeof(image)-flen;
		memmove(image,new_start, sizeof(image)-flen);
		fread(new_end, flen, 1, stdin);
		frame=frame+1;
	}
}

