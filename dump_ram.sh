#!/bin/bash
OFFSET=0x40000     #Address offset
LENGTH=0x7FFF      #Length to read in words
IMAGE=/tmp/xmem    #Dump output prefix

OCD_HOST=127.0.0.1
OCD_PORT=4444
KHZ=500            #56L307 connection speed 500Khz

bytes=$(echo "ibase=16;$(echo $LENGTH | tr -d '0x') * 4" | bc)

#We must keep the telnet session open for the duration of the memory dump
#Transfer speed at 500Khz is approx 10KiB/s
speed=$(echo "10 * 1000 * $KHZ / 500" | bc)
delay=$(echo "$bytes / $speed" | bc)
(
echo halt
echo set memspace 1
echo dump_image ${IMAGE}_raw ${OFFSET} ${bytes}
sleep ${delay}
) | telnet ${OCD_HOST} ${OCD_PORT}

#Dump image as hex; for visual inspection
xxd -p -g 4 -c 4 ${IMAGE}_raw > ${IMAGE}_hex

#Compile process_dump
gcc process_dump.c -o process_dump

#Convert dump to fractional representation
./process_dump ${IMAGE}_raw > ${IMAGE}_data

echo Raw: ${IMAGE}_raw
echo Hex: ${IMAGE}_hex
echo Fractional: ${IMAGE}_data
#Plot
gnuplot -persist -e "plot '${IMAGE}_data' using 1"
