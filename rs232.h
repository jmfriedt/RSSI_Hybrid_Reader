#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>                  /* declaration of bzero() */
#include <fcntl.h>
#include <termios.h>
int init_rs232();
void free_rs232();
void sendstr(int,char*);
int  readstr(int,char*,int);
struct termios oldtio,newtio;

#define BAUDRATE B115200
#define RS_DEVICE "/dev/ttyAMA0"

