#include <stdio.h>
#include <stdlib.h>
#include "rs232.h"
#include <ctype.h>
#include <unistd.h>

#define DEBUG printf
// #define DEBUG nothing

int main(int argc,char **argv)
{int fd,i,j,k,n,length;FILE *f;
 long val;
 char c[256];
 char* filename=NULL;
 int cmd;
 int power = 40, fstart= 2405, samples=250, fstep=250, averages=40;

 while ((cmd = getopt (argc, argv, "f:n:p:s:o:a:")) != -1) 
   switch (cmd)
     {case 'f':
        fstart=atoi(optarg);
        break;
      case 'n':
        samples=atoi(optarg);
        break;
      case 'p':
        power = atoi(optarg);
        break;
      case 's':
        fstep = atoi(optarg);
        break;
      case 'o':
        filename=optarg;
        break;
      case 'a':
        averages=atoi(optarg);
        break;
      default:
        printf("%s [-f fstart] [-n samples] [-p power] [-s fstep] [-o filename]\n",argv[0]);
     }
 fd=init_rs232(); 

// get reader ID so we known who we are talking to
 sprintf(c,"vv\r\n");  // \r = CR
 sendstr(fd,c);
 n=readstr(fd,c,7);
 
 printf("%d chars received\n",n);
 for (i=0;i<n;i++)
     {DEBUG("%c",c[i]); 
      if (c[i]=='\r') DEBUG("\n");
     }
 printf("\n");

// configure reader 
 printf("cmd a012\n");
 sprintf(c,"a012\r\n"); // mono static antenna 1
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);
 
 printf("cmd I0\n");
 sprintf(c,"I0\r\n");   // tty0 (default) -> tty1
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);
 
 printf("cmd B0060\n");
 // sprintf(c,"B0180\r\n");// data format (see Python configuration): Im Re of high pass
 sprintf(c,"B0060\r\n");   // data format -- NO high pass
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);

 sprintf(c,"4%04d\r\n",fstart); // start frequency
 printf("cmd %s\n",c);
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);
 
 sprintf(c,"5%04d\r\n",fstep);// frequency step (250 kHz) = 1/4 us
 printf("cmd 50050\n");
 sendstr(fd,c); 
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);

 sprintf(c,"3%04d\r\n",samples);// number of samples: 250 kHz*250 samples = 50 MHz
 printf("cmd %s\n",c);
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);
 
 sprintf(c,"g1%02d\r\n",power);// TX power
 printf("cmd %s\n",c);
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);
 
 sprintf(c,"2%04d\r\n",averages);// averages
 printf("cmd %s\n",c);
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);

 printf("cmd 11\n");
 sprintf(c,"11\r\n");   // transmitter on
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);

 printf("cmd x2\n");
 sprintf(c,"x2\r\n");   // single sweep
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);

 if (filename==NULL) 
    f=fopen("output.txt","w");
    else f=fopen(filename,"w");
 i=0;k=0;
 do {read(fd,&c[0],1);
     if (i==0) {if (c[0]!='d')  printf("Header error\n");}
     if (i==1) {if (c[0]!=0x0c) printf("Antenna error\n");}
     if (i==2) {printf("Mode: %x ",c[0]);}
     if (i==3) {printf("%x\n",c[0]);}
     if (i==4) {printf("Words %x ",c[0]);length=c[0];}
     if (i==5) 
        {length=length+((((long)c[0])&0xff)<<8);printf("%x=%d\n",c[0],length);
         for (j=0;j<length;j++) 
             {read(fd,&c[0],1);   val=(((long)c[0])&0xff);        // LSB
              read(fd,&c[0],1);   val+=((((long)c[0])&0xff)<<8);  
              read(fd,&c[0],1);   val+=((((long)c[0])&0xff)<<16); // MSB
              if ((val&0x800000)!=0) val+=(((long)0xff)<<24); // complement for sign
              fprintf(f,"%ld\n",val);
             }
         read(fd,&c[0],1); printf("CRC %x",c[0]); // checksum
        }
     i++; if (i==6) {i=0;k++;fprintf(f,"\n");}    // next data block starting again with header ...
     if (i>5) printf("%d: %x ",i,c[0]);
 } while (k<2);

 fclose(f);

 printf("cmd 10\n");
 sprintf(c,"10\r\n");   // transmitter off
 sendstr(fd,c);
 do {read(fd,&c[0],1); 
     if (c[0]==6) printf("ACK\n"); else printf ("ACK not received: %x\n",c[0]);
    } while (c[0]!=6);
}
