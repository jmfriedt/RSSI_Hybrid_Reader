#include <stdio.h>
#include <stdlib.h>
#include "rs232.h"

#define DEBUG printf
// #define DEBUG nothing

int main(int argc,char **argv)
{int fd;
 char cout[12],cin;
 fd=init_rs232(); 
 sprintf(cout,"vv\r\n");  // \r = CR
 sendstr(fd,cout);
 printf("%s sent\n",cout);
 do {read(fd,&cin,1);
     if ((cin>31) && (cin<128)) 
	DEBUG("%c",(cin&0xff)); 
     else if (cin=='\n') printf("\\n\n");
	  else DEBUG("(%x) ",(cin));
     fflush(stdout);
 } while (cin != '\n');
}
