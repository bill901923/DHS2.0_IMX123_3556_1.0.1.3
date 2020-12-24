#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/signal.h>
#include  <sys/stat.h>
#include  <fcntl.h>
#include  <termios.h>
#include  <errno.h>
#include  <limits.h>
#include  <string.h>
#include <sys/select.h>
#include   "dhs_uart.h"
int raudRate( int baudrate)
{
    switch(baudrate)
    {
    case 0:
        return (B2400);
    case 1:
        return (B4800);
    case 2:
        return (B9600);
    case 3:
        return (B19200);
    case 4:
        return (B38400);
    case 5:
        return (B57600);
    case 6:
        return (B115200);
    default:
        return (B9600);
    }
}

//int setPara(int serialfd,int speed=2,int databits=8,int stopbits=1,int parity=0)
int setPara(int serialfd,int speed,int databits,int stopbits,int parity)
{
    struct termios termios_new;
    bzero( &termios_new, sizeof(termios_new));//µ»º€”⁄memset(&termios_new,sizeof(termios_new));
    cfmakeraw(&termios_new);//æÕ «Ω´÷’∂À…Ë÷√Œ™‘≠ ºƒ£ Ω
    termios_new.c_cflag=raudRate(speed);
    termios_new.c_cflag |= CLOCAL | CREAD;
  //  termios_new.c_iflag = IGNPAR | IGNBRK;

    termios_new.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 0:
        termios_new.c_cflag |= CS5;
        break;
    case 1:
        termios_new.c_cflag |= CS6;
        break;
    case 2:
        termios_new.c_cflag |= CS7;
        break;
    case 3:
        termios_new.c_cflag |= CS8;
        break;
    default:
        termios_new.c_cflag |= CS8;
        break;
    }

    switch (parity)
    {
    case 0:  				//as no parity
        termios_new.c_cflag &= ~PARENB;    //Clear parity enable
      //  termios_new.c_iflag &= ~INPCK; /* Enable parity checking */  //add by fu
        break;
    case 1:
        termios_new.c_cflag |= PARENB;     // Enable parity
        termios_new.c_cflag &= ~PARODD;
        break;
    case 2:
        termios_new.c_cflag |= PARENB;
        termios_new.c_cflag |= ~PARODD;
        break;
    default:
        termios_new.c_cflag &= ~PARENB;   // Clear parity enable
        break;
    }
    switch (stopbits)// set Stop Bit
    {
    case 1:
        termios_new.c_cflag &= ~CSTOPB;
        break;
    case 2:
        termios_new.c_cflag |= CSTOPB;
        break;
    default:
        termios_new.c_cflag &= ~CSTOPB;
        break;
    }
    tcflush(serialfd,TCIFLUSH); // «Â≥˝ ‰»Îª∫¥Ê
   tcflush(serialfd,TCOFLUSH); // «Â≥˝ ‰≥ˆª∫¥Ê
    termios_new.c_cc[VTIME] = 1;   // MIN”Î TIME◊È∫œ”–“‘œ¬Àƒ÷÷£∫1.MIN = 0 , TIME =0  ”–READ¡¢º¥ªÿ¥´ ∑Ò‘Ú¥´ªÿ 0 ,≤ª∂¡»°»Œ∫Œ◊÷‘™
    termios_new.c_cc[VMIN] = 1;  //    2°¢ MIN = 0 , TIME >0  READ ¥´ªÿ∂¡µΩµƒ◊÷‘™,ªÚ‘⁄ Æ∑÷÷Æ“ª√Î∫Û¥´ªÿTIME »Ù¿¥≤ªº∞∂¡µΩ»Œ∫Œ◊÷‘™,‘Ú¥´ªÿ0
    tcflush (serialfd, TCIFLUSH);  //    3°¢ MIN > 0 , TIME =0  READ ª·µ»¥˝,÷±µΩMIN◊÷‘™ø…∂¡
    return tcsetattr(serialfd,TCSANOW,&termios_new);  //    4°¢ MIN > 0 , TIME > 0 √ø“ª∏Ò◊÷‘™÷Æº‰º∆ ±∆˜º¥ª·±ª∆Ù∂Ø READ ª·‘⁄∂¡µΩMIN◊÷‘™,¥´ªÿ÷µªÚ
}

int writeData(int fd,const char *data, int datalength )
{
    if(fd <0){ return -1;}
    int len = 0, total_len = 0;//modify8.
    for (total_len = 0 ; total_len < datalength;)
    {
        len = 0;
        len = write(fd, &data[total_len], datalength - total_len);
        if((len!=strlen(data))&&((len!=strlen(data)-2)))
        {
            printf("WriteData fd = %d ,len =%d,data = ",fd,len);
            for(int in_i=0;in_i<len;in_i++)
            {
                printf("%02X ",data[in_i]);    
            }
                printf("\n"); 
                
        }
        else
        {
            printf("WriteData fd = %d ,len =%d,data = %s\n",fd,len,data);
        }
        
        
        if (len > 0)
        {
            total_len += len;
        }
        else if(len <= 0)
        {
            len = -1;
            break;
        }
     }
       return len;
}
int readData(int fd,char *data, int datalength)
{
       if(fd <0){ return -1;}
       int len = 0;
       memset(data,0,datalength);

       int max_fd = 0;
       fd_set readset ={0};
       struct timeval tv ={0};

       FD_ZERO(&readset);
       FD_SET((unsigned int)fd, &readset);
       max_fd = fd +1;
       tv.tv_sec=0;
       tv.tv_usec=1000;
       if (select(max_fd, &readset, NULL, NULL,&tv ) < 0)
       {
               printf("ReadData: select error\n");
       }
       int nRet = FD_ISSET(fd, &readset);
      if (nRet)
       {
              len = read(fd, data, datalength);
       }
       return len;
}

void closePort(int fd)
{
    struct termios termios_old;
    if(fd > 0)
    {
        tcsetattr (fd, TCSADRAIN, &termios_old);
        close (fd);
    }
}





int  openPort( char *device)
{
    struct termios termios_old;
    int fd;
    fd = open( device, O_RDWR | O_NOCTTY |O_NONBLOCK);//O_RDWR | O_NOCTTY | O_NDELAY   //O_NONBLOCK
    if (fd < 0)
    { return -1;}
    tcgetattr(fd , &termios_old);
    return fd;
}





