
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>



#define SB_GPIO_BASE  0x4800

#define CS5536_DEBUG 1

#define  read_reg( addr, var )  	var = *(volatile unsigned int *)(addr)

#define  set_reg_bit( addr, n )    							\
			{												\
				unsigned int  midvar = 0;					\
				midvar = *(volatile unsigned int *)(addr);	\
				midvar &= ~(0x00010000 << (n));				\
				midvar |= (0x00000001 << (n));				\
															\
				*(volatile unsigned int *)(addr) = midvar;	\
			}

#define  clear_reg_bit( addr, n )    						\
			{												\
				unsigned int  midvar = 0;					\
				midvar = *(volatile unsigned int *)(addr);	\
				midvar |= (0x00010000 << (n));				\
				midvar &= ~(0x00000001 << (n));				\
															\
				*(volatile unsigned int *)(addr) = midvar;	\
			}
			
			

int arg_filter( int argc, char *argv[] );
void set_timer0( int );
void set_timer1( int );
void light_on(void);
void light_off(void);
void light_led_continuously( int);
void light_led_blinking(int);


unsigned int iobaseaddr = 0;
unsigned int gpioaddr = 0;
unsigned int count = 0;
unsigned int time = 0;


int main( int argc, char *argv[] )
{
	int fd = 0;
	int i = 0;
	int retval = -1;	
	int interval = 1;
	
	if ((retval = arg_filter( argc, argv)) != 0) {
		return -1;
	}

	if ((fd = open("/dev/mem", O_RDWR)) < 0) {
		perror("open");
		return -1;
	}
	
	iobaseaddr = mmap( 0,
				0x100000,
				PROT_READ|PROT_WRITE,
				MAP_SHARED,
				fd,
				(unsigned int)0x1fd00000 );

	gpioaddr = iobaseaddr + SB_GPIO_BASE;

	time = atoi(argv[2]);

	// Register function to SIGALRM
	if ( !(strcmp(argv[1], "0")) ) {
		light_on();
		signal(SIGALRM, light_led_continuously);
		set_timer0( time );
	}
	else if ( !(strcmp(argv[1], "1")) ) {
		light_on();
		interval = 200000;
		signal(SIGALRM, light_led_blinking);
		set_timer1( interval );
	}

	while (count < time) {
		pause();
	}

	return 0;
}


int arg_filter( int argc, char *argv[] )
{
	char *cmd_options = "\n"
                        "isservok option time\n"
						"--------------------------------------------\n"
						"option:\t 0 -- light continuously,\n"
						"\t 1-- blinkling. \n"
						"\n"
						"time:\t > 0";


	if (argc < 3) {
		printf("Not enough arguments.\n");
		printf("%s\n", cmd_options);
		return -1;
	}
	else if (argc > 3) {
		printf("Too many arguments.\n");
		printf("%s\n", cmd_options);
		return -1;
	}
	
	if (atoi(argv[1]) != 0 && atoi(argv[1]) != 1 ) {
		printf("Invalid option argument.\n");
		return -1;
	}
	
	if (atoi(argv[2]) == 0) {
		printf("Invalid time argument.\n");
		return -1;
	}
	
	return 0;

}


void set_timer0( int n )
{

	// Get system call result to determine successful or failed  
	int res = 0;
	struct itimerval tick;  

	// Initialize struct  
	memset(&tick, 0, sizeof(tick));  

	// Timeout to run function first time  
	tick.it_value.tv_sec = n;  // sec  
	tick.it_value.tv_usec = 0; // micro sec.  

	// Interval time to run function  
	tick.it_interval.tv_sec = 0;  
	tick.it_interval.tv_usec = 0;  

	// Set timer, ITIMER_REAL : real-time to decrease timer,  
	//                          send SIGALRM when timeout  
	res = setitimer(ITIMER_REAL, &tick, NULL);

	if (res) {  
		printf("Set timer failed!!\n");  
	}  

}

void set_timer1( int interval)
{

	// Get system call result to determine successful or failed  
	int vtime = time;
	int utime = interval;
    int res = 0;
	struct itimerval tick;  

    // 0.5s as bech
    vtime = 500000 * time / interval;
    time = vtime;
	// Initialize struct  
	memset(&tick, 0, sizeof(tick));  

	// Timeout to run function first time  
	tick.it_value.tv_sec = 0;  // sec  
	tick.it_value.tv_usec = interval; // micro sec.  

	// Interval time to run function  
	tick.it_interval.tv_sec = 0;  
	tick.it_interval.tv_usec = interval;  

	// Set timer, ITIMER_REAL : real-time to decrease timer,  
	//                          send SIGALRM when timeout  
	res = setitimer(ITIMER_REAL, &tick, NULL);

	if (res) {  
		printf("Set timer failed!!\n");  
	}  

}

void light_on()
{
	set_reg_bit( (gpioaddr + 0x00), 0x07 );
	set_reg_bit( (gpioaddr + 0x04), 0x07 );
	
	return;
}

void light_off()
{
	clear_reg_bit( (gpioaddr + 0x00), 0x07 );
	clear_reg_bit( (gpioaddr + 0x04), 0x07 );
	
	return;
}

void light_led_continuously( int sig)
{
	light_off();
	count += time;
	
	return;
}


void light_led_blinking( int sig )
{
	unsigned int tmp;
	
	read_reg( (gpioaddr + 0x00), tmp );
	if (tmp & 0x00000080) {
		light_off();	
    	count++;
	}
	else {
		light_on();
	}


	return;
}
