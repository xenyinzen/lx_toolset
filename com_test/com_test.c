/*
 * This file give the basic com sending and receiving data example
 *
 * Modified by Tang Gang. 
 * 2008.8.5
 */

#include  <stdio.h>      
#include  <stdlib.h>     
#include  <unistd.h>    
#include  <sys/types.h>  
#include  <sys/stat.h>  
#include  <fcntl.h>      
#include  <termios.h>    
#include  <errno.h>      
#include  <string.h>
#include  <math.h>

/*
 * Macro Defines
 */
#define DEBUG 0

#define BUFFER_LENGTH 1024

#define COUNT	10

#define SELECT_PORT		0
#define SERIAL_DEV0		"/dev/ttyS0"
#define SERIAL_DEV1		"/dev/ttyS1"

#define BAND_RATE		115200

/* Global variables */
char *sample_str = "01234567";
char buffer[BUFFER_LENGTH];			// to store recieved data

/* Bandrate data */
static int speed_arr[] = { 
B921600, B460800, B230400, B115200, B57600, 
B38400, B19200, B9600, B4800, B2400, 
B1200, B300, B38400, B19200, B9600, 
B4800, B2400, B1200, B300 };

static int band_arr[] = {
921600, 460800, 230400, 115200, 57600, 
38400,  19200,  9600,  4800,  2400,  
1200,  300, 38400,  19200,  9600, 
4800, 2400, 1200,  300 };



/* Function Declaration */
int openDev( char *Dev );
void set_speed( int fd, int speed );
int	set_Parity( int fd, int databits, int stopbits, int parity );


/* Entry */
int main(int argc, char *argv[])
{
	int fd;
	char buf[BUFFER_LENGTH];
	char *dev;
	int device, i;
	int flag = 0;
	int res = 0;
	
	device = SELECT_PORT;
	if (device == 0) {
		dev = SERIAL_DEV0;
	} 
	else if (device == 1) {
		dev = SERIAL_DEV1;
	}
		
	if ((fd = openDev(dev)) <= 0 ) {
		printf( "Can't Open Serial Port!\n" );
		return -1;
	}	
	
	set_speed(fd, BAND_RATE);
	
	if (set_Parity(fd, 8, 1, 'N') == -1) {
		printf( "Set Parity Error\n" );
		return -1;
	}
	
	for (i = 0; i < COUNT; i++ ) {		
		write( fd, sample_str, strlen(sample_str) );
	    
	    do {
	    	res = read( fd, buffer, strlen(sample_str) );
	    			printf("Buffer: %s\n", buffer);
	    } while (res == 0);

		printf("Buffer: %s\n", buffer);
		printf("Buf: %s\n", sample_str);
				
		res = strcmp( buffer, sample_str );
		if (res != 0) {
			flag = 1;
			break;
		}
		/* clear whole buffer */
	    memset( buffer, 0, BUFFER_LENGTH );
	}
	
	if (flag == 1) {
		
		printf("COM test error.\n");	
		return -1;	
	}
	
	printf("COM test successed.\n");	
	close(fd);
	return 0;
}



/*   
 * desc:	open serial
 *	
 * in:		Dev -- pointer to the device of serial port	
 *			
 * out:		== -1, error. == others, ok. 
 */
int openDev(char *Dev)
{
	int fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY
	if (-1 == fd) { 
		perror("Can't Open Serial Port");
		return -1;
	}
		
	return fd;
}

/* desc:	set the communication rate of serial port
 *
 * in:		fd -- file handler. speed -- communication rate
 *			
 * out:		void
 */
void set_speed(int fd, int speed)
{
	int   i;
	int   status;
	struct termios   Opt;
	
	tcgetattr(fd, &Opt);
	for (i= 0; i < sizeof(speed_arr) / sizeof(int); i++) {
		if (speed == band_arr[i]) {
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if (status != 0)
				perror("tcsetattr fd1");
			return;
		}
		tcflush(fd, TCIOFLUSH);
	}
}

/* desc:	set the data's parity
 *
 * in:		fd -- device handler.
 *			databits -- data length (by bits)
 *			stopbits -- the stop bits length
 *			parity -- the parity char pattern
 * out:		== -1, error. 
 *			== 0, ok.
 */
int set_Parity(int fd, int databits, int stopbits, int parity)
{               
	struct termios options;
	
	if (tcgetattr(fd,&options) != 0) {
		perror("SetupSerial 1");
		return -1;
	}
	
	options.c_cflag &= ~CSIZE;
	switch (databits) {
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unsupported data size\n");
			return -1;
	}
	
	switch (parity) {
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/ 
			options.c_iflag |= INPCK;             /* Disnable parity checking */
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
			return -1;
	}
	/* 设置停止位*/   
	switch (stopbits) {
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported stop bits\n");
			return -1;
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
		return -1;
	}
	return 0;
}


