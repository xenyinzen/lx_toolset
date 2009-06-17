#include  <stdio.h>      
#include  <stdlib.h>     
#include     <unistd.h>    
#include     <sys/types.h>  
#include     <sys/stat.h>  
#include     <fcntl.h>      
#include     <termios.h>    
#include     <errno.h>      

#define FALSE -1
#define TRUE 0

char *recchr="\nWe received:\n";
char buffer[200];
char *buf="01234567\n";
int res;
int speed_arr[] = { B921600, B460800, B230400, B115200, B57600, B38400, B19200, B9600, B4800, 
			B2400, B1200, B300, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {921600, 460800, 230400, 115200, 57600, 38400,  19200,  9600,  4800,  2400,  
			1200,  300, 38400,  19200,  9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed)
{
		int   i;
		int   status;
		struct termios   Opt;
		tcgetattr(fd, &Opt);
		for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
		{
				if  (speed == name_arr[i])
				{
						tcflush(fd, TCIOFLUSH);
						cfsetispeed(&Opt, speed_arr[i]);
						cfsetospeed(&Opt, speed_arr[i]);
						status = tcsetattr(fd, TCSANOW, &Opt);
						if  (status != 0)
								perror("tcsetattr fd1");
						return;
				}
				tcflush(fd,TCIOFLUSH);
		}
}


int set_Parity(int fd,int databits,int stopbits,int parity)
{               
		struct termios options;
		if  ( tcgetattr( fd,&options)  !=  0)
		{
				perror("SetupSerial 1");
				return(FALSE);
		}
		options.c_cflag &= ~CSIZE;
		switch (databits) /*设置数据位数*/
		{
				case 7:
						options.c_cflag |= CS7;
						break;
				case 8:
						options.c_cflag |= CS8;
						break;
				default:
						fprintf(stderr,"Unsupported data size\n");
						return (FALSE);
		}
		switch (parity)
		{
				case 'n':
				case 'N':
						options.c_cflag &= ~PARENB;   /* Clear parity enable */
						options.c_iflag &= ~INPCK;     /* Enable parity checking */
						break;
				case 'o':
				case 'O':
						options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/ 
						options.c_iflag |= INPCK;           /* Disnable parity checking */
						break;
				case 'e':
				case 'E':
						options.c_cflag |= PARENB;     /* Enable parity */
						options.c_cflag &= ~PARODD;   /* 转换为偶效验*/  
						options.c_iflag |= INPCK;       /* Disnable parity checking */
						break;
				case 'S':
				case 's':  /*as no parity*/
						options.c_cflag &= ~PARENB;
						options.c_cflag &= ~CSTOPB;
						break;
				default:
						fprintf(stderr,"Unsupported parity\n");
						return (FALSE);
		}
		/* 设置停止位*/   
		switch (stopbits)
		{
				case 1:
						options.c_cflag &= ~CSTOPB;
						break;
				case 2:
						options.c_cflag |= CSTOPB;
						break;
				default:
						fprintf(stderr,"Unsupported stop bits\n");
						return (FALSE);
		}
		/* Set input parity option */
		if (parity != 'n')
				options.c_iflag |= INPCK;
		options.c_cc[VTIME] = 150; // 15 seconds
		options.c_cc[VMIN] = 0;

		tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
		if (tcsetattr(fd,TCSANOW,&options) != 0)
		{
				perror("SetupSerial 3");
				return (FALSE);
		}
		return (TRUE);
}

/*open serial*/ 
int OpenDev(char *Dev)
{
		int fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY
		if (-1 == fd)
		{ /*设置数据位数*/
				perror("Can't Open Serial Port");
				return -1;
		}
		else
				return fd;

}
int main(int argc, char *argv[])
{
	int fd;
	int nread;
	char buff[512];
	char *dev ="/dev/ttyS0";

	int device, c;
	//sleep(1);
	fd = OpenDev(dev);

	if (fd>0){
		set_speed(fd,115200);
	}
	else{
		printf("Can't Open Serial Port!\n");
		return -1;
	}
	if (set_Parity(fd,8,1,'N')== FALSE)
	{
		printf("Set Parity Error\n");
		return -1;
	}
	int num = 0;
	while(num < 3)
	{
	if(device==0){
	            write(fd,buf,9);
		    do {
		    	res=read(fd,buffer,9);
		    } while (res == 0);
		    res = strcmp(buffer,buf);
		    printf("%s\n",buffer);
		    
		    if(device==1)write(fd,"12345678\n",9);
		/*   
		 *   if(device==2)write(fd,"23456789\n",9);
		 *   if(device==3)write(fd,"3456789a\n",9);
		 *   if(device==4)write(fd,"456789ab\n",9);
		 *   if(device==5)write(fd,"56789abc\n",9);
		 *   if(device==6)write(fd,"6789abcd\n",9);
		 *   sleep(1);
		 */
	}
	num++;
	}
	close(fd);
	
	/* success */ 
	return 0;
}




