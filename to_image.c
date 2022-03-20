#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define SRATE 2000000.0

#define PIXELS 60162048


#define OLINES (625)

#define TIMGSIZE (OLINES*256)

int main(int argc, char *argv[])
{
	if (argc<2) {
		printf("Usage: %s <infile>\n infile has decoded values as double floats\n", argv[0]);
		return 1;
	}

	FILE *f=fopen(argv[1], "r");
	if (f==NULL) {
		printf("Couldn't open file %s\n", argv[1]);
		return 1;
	}
	fseek(f, 0, SEEK_END);
	size_t fsize=ftell(f);
	fseek(f, 0, SEEK_SET);
	double *image=malloc(fsize);
	if (image==NULL) {
		printf("Couldn't malloc\n");
		return 1;
	}
	fsize=fread(image, 1, fsize, f);
	fclose(f);

	int num_pixels=fsize/sizeof(double);
	printf("%ld octetts read => %d pixels\n", fsize, num_pixels);

	int n=0;
	double t=0;
	double i=0;
	
	int frames=num_pixels/(313*128);
	printf("%d frames\n", frames);
	int frame=0;
	for (frame=0; frame<frames; frame=frame+1){
		int fpos=round(frame*312.5)*(128);
		int line;

		int avgf=10; //How many fields to average over
		int avgl=1; //How many lines to average over

		double fimg[TIMGSIZE];
		for (line=0; line<OLINES; line++) {
			int lpos=fpos+128*line;
			int px=0;
			for (px=0; px<256; px++) {
				int ppos=lpos+px;
				double sum=0;
				int fa;
				for (fa=-avgf/2; fa<=-avgf+avgf; fa++) {
					int la;
					for (la=-avgl/2; la<-avgl/2+avgl; la++) {
						int epos=ppos+(la+round(fa*312.5))*128;
						if (epos<0) continue;
						if (epos>=num_pixels) continue;
						sum=sum+image[epos];
					}
				}
				fimg[line*256+px]=sum;
			}
		}

		double max=0;
		double min=99999;
		int px=0;
		for (px=4; px<256-4; px++) {
			double sum=0;
			int cnt=0;
			for (line=0; line<OLINES; line++) {
				int x=0;
				for (x=-4;x<=4; x++){ 
					sum=sum+fimg[line*256+px+x];
					cnt=cnt+1;
				}
			}
			sum=sum/cnt;
			if (sum>max) max=sum;
			if (sum<min) min=sum;
		}

		printf("Frame %d of %d; min=%lf max=%lf\n", frame, frames, min, max);

		uint8_t iimg[TIMGSIZE];
		for (line=0; line<625; line++) {
			int px=0;
			for (px=0; px<256; px++) {
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
		FILE *f=fopen(fname, "w");	
		memset(fname, 0, sizeof(fname));
		snprintf(fname, sizeof(fname)-1, "P5 256 625 255\n");
		fwrite(fname, strlen(fname), 1, f);
		fwrite(iimg, sizeof(iimg), 1, f);
		fclose(f);
	}
	free(image);
}

