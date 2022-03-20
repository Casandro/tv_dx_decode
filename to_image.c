#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define SRATE 2000000.0

#define PIXELS 60162048

double image[PIXELS];

#define OLINES (625)

#define TIMGSIZE (OLINES*256)

int main(int argc, char *argv[])
{
	int n=0;
	double t=0;
	double i=0;
	fread(image, sizeof(image), 1, stdin);
	
	int frames=PIXELS/(313*128);
	int frame=0;
	for (frame=0; frame<frames; frame=frame+1){
		int fpos=round(frame*312.5)*(128);
		int line;

		int avgf=1;
		int avgl=1;

		double fimg[TIMGSIZE];
		for (line=0; line<OLINES; line++) {
			int lpos=fpos+128*line;
			int px=0;
			for (px=0; px<256; px++) {
				int ppos=lpos+px;
				double sum=0;
				int fa;
				for (fa=-avgf; fa<=avgf; fa++) {
					int la;
				//	for (la=-avgl; la<=avgl; la++) {
						int epos=ppos+(la+round(fa*312.5))*128;
						if (epos<0) continue;
						if (epos>=PIXELS) continue;
						sum=sum+image[epos];
				//	}
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
}

