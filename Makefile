 all: rssi_sdm

rssi_sdm: rssi_sdm.o rs232.o
	arm-linux-gcc -o rssi_sdm rssi_sdm.o rs232.o

rssi_sdm.o: rssi_sdm.c
	arm-linux-gcc -Wall -O2 -c rssi_sdm.c

rs232.o: rs232.c rs232.h
	arm-linux-gcc -Wall -O2 -c rs232.c

clean:
	rm *.o rssi_sdm
