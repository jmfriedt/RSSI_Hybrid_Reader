# make sure output/host/bin of Buildroot for RPi2 (make raspberrypi2_defconfig)
# is in the PATH. The program is statically compiled to avoid dependence with 
# dynamic libraries on the target board

all: rssi_sdm

LDFLAGS=-static
CFLAGS=-Wall -O2

rssi_sdm: rssi_sdm.o rs232.o
	arm-linux-gcc $(LDFLAGS) -o rssi_sdm rssi_sdm.o rs232.o

rssi_sdm.o: rssi_sdm.c
	arm-linux-gcc $(CFLAGS) -c rssi_sdm.c

rs232.o: rs232.c rs232.h
	arm-linux-gcc $(CFLAGS) -c rs232.c

clean:
	rm *.o rssi_sdm
