#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#define SB_GPIO_BASE  0x4800
#define SHORT 500
#define	LONG  1000
	
unsigned int gpioaddr;
unsigned int value; 								// contiue times
unsigned int count; 								//count
int time = 0;										//time
int mark = 0, mark_sub = 0;

int arg_filter( int argc, char *argv[]);
int set_timer(void);
void beep(int time);

int main( int argc, char *argv[] )
{
	int fd = 0;
	unsigned int retval = 0,iobaseaddr = 0;
		
	if ((retval = arg_filter( argc, argv)) != 0) 
	{
		return -1;
	}

	if ((fd = open("/dev/mem", O_RDWR)) < 0) 
	{
		perror("open");
		return -1;
	}
	
	iobaseaddr = mmap( 0,
				0x100000,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				fd,
				(unsigned int)(0x1fd00000) );
	
	gpioaddr = iobaseaddr + SB_GPIO_BASE;

	*(volatile unsigned int *)(gpioaddr + 0x00) = 0xffff0000;
   	*(volatile unsigned int *)(gpioaddr + 0x10) = 0x2fffd000;
   	*(volatile unsigned int *)(gpioaddr + 0x04) = 0x2ffdd002;
	
	time = atoi(argv[2]); 

	if ( !(strcmp(argv[1], "0")) ) 
	{
		mark = 0; 
		signal(SIGALRM, beep);
		set_timer();
	}

	if ( !(strcmp(argv[1], "1")) ) 
	{
		mark = 1;
		signal(SIGALRM, beep);
		set_timer();
	}

	if(!(strcmp(argv[1], "2")) )
	{
		mark = 2;
		signal(SIGALRM, beep);
		set_timer();	
	}

	for(;;)
	{
		if(count > time) 
			break;
	}
}

void beep(int time)
{
   	int i, j,num = 0;
	   
	if(mark == 0)  //short_beep
		value = SHORT;
	if(mark == 1) //long_beep
		value = LONG;

	// short_beep  && long_beep
	if(mark == 2 && mark_sub == 0)
	{
		value = SHORT;
		mark_sub = 1;
	} 
	else if(mark == 2 && mark_sub == 1)
	{
		value = LONG;
		mark_sub = 0;
	}

	while(1) 
	{
       	*(volatile unsigned int *)(gpioaddr + 0x00) = 0xfffd0002;
        for ( i=0; i < 1000; i++) 
		{ 
			for(  j=0; j < 10; j++); 
		}
       	*(volatile unsigned int *)(gpioaddr + 0x00) = 0xffff0000;
        for ( i=0; i < 1000; i++) 
		{
			for(  j=0; j < 10; j++); 
		}
		if(num > value)
			break;
		num ++;
    }
	count ++ ;							// + 1 sec
	signal(SIGALRM, beep);
}

int set_timer(void)
{
	int res = 0;
	struct itimerval tick;  

	memset(&tick, 0, sizeof(tick));  

	tick.it_value.tv_sec = 1;  // sec  
	tick.it_value.tv_usec = 0; // micro sec.  
	tick.it_interval.tv_sec = 1;  //interal 1 sec
	tick.it_interval.tv_usec = 0;  

	res = setitimer(ITIMER_REAL, &tick, NULL);
	if (res)
	{  
		printf("Set timer failed!!\n");  
	}  
}

int arg_filter( int argc, char *argv[] )
{
	char *cmd_options = "\n"
                        "beep option time\n"
						"--------------------------------------------\n"
						"option:\t 0 -- short beep.\n"
						"\t 1 -- long beep.\n"
						"\t 2 -- short & long beep.\n"
						"\n"
						"time:\t > 0";


	if (argc < 3)
	{
		printf("Not enough arguments.\n");
		printf("%s\n", cmd_options);
		return -1;
	}
	else if (argc > 3)
	{
		printf("Too many arguments.\n");
		printf("%s\n", cmd_options);
		return -1;
	}
	
	if (atoi(argv[1]) != 0 && atoi(argv[1]) != 1 && atoi(argv[1]) != 2 )
	{
		printf("Invalid option argument.\n");
		return -1;
	}
	
	if (atoi(argv[2]) == 0)
	{
		printf("Invalid time argument.\n");
		return -1;
	}
	
	return 0;

}
