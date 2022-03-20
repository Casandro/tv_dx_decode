#!/bin/bash

inputfile=$1

if [ ! -f "$inputfile" ]
then
	echo "No input file given"
	exit
fi

for x in *.c
do
	echo compiling $x
	gcc -O3 -o `basename $x .c` $x -lm || exit
done

mkdir -p images
rm images/IMG*.pgm
echo "demodulating signal"
./demod -52e3 < $inputfile > /tmp/temp.dat
echo "converting to images"
cd images 
../to_image /tmp/temp.dat
ffmpeg -r 50 -f image2 -s 256x625  -i IMG-%08d.pgm -filter scale=896:625,pad=896:626  -c:v libx264 -crf 25  -pix_fmt yuv420p `basename $inputfile .wav`.avi
cd ..

