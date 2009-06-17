#include <stdio.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(void) 
{
	FILE *p_file = NULL;
    int i, j, fd, retval, irqcount = 0;
    unsigned long tmp, data;
    struct rtc_time rtc_tm, rtc_tm2;

	if ((p_file = fopen( "./rtc_data.txt", "w")) == NULL ) {
		fprintf(stderr, "Error. Can not creat rtc_data.txt.\n" );
		return -1;
	}

	/* open the rtc device */
    fd = open ("/dev/rtc", O_RDWR);
    if (fd ==  -1) {
		perror("/dev/rtc");
		exit(errno);
    }

	/* read the rtc time first, and print it out */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    if (retval == -1) {
		perror("ioctl");
		exit(errno);
    }

  	fprintf( stderr, "%d-%d-%d, %02d:%02d:  %02d.\n",
    	rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	/* prepare the rtc time struct variable, set the rtc time manually */
	rtc_tm.tm_mday = 1;
	rtc_tm.tm_mon = 12 - 1;
	rtc_tm.tm_year = 2008 - 1900;
	rtc_tm.tm_hour = 0;
	rtc_tm.tm_min = 0;
	rtc_tm.tm_sec = 0;

    /* Set the RTC time/date, and print it out */
    retval = ioctl(fd, RTC_SET_TIME, &rtc_tm);
    if (retval == -1) {
		perror("ioctl");
		exit(errno);
    }

    fprintf( stderr, "%d-%d-%d, %02d:%02d:  %02d.\n",
    	rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	/* delay for a while */
	//for (i=0; i<10000UL; i++)
	//	for (j=0; j<1000UL; j++){}

	sleep(5);

	/* read it again */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm2);
    if (retval == -1) {
		perror("ioctl");
		exit(errno);
    }

	/* compare between the just read value and the value written */
	if (rtc_tm2.tm_mday == rtc_tm.tm_mday
			&& rtc_tm2.tm_mon == rtc_tm.tm_mon
			&& rtc_tm2.tm_year == rtc_tm.tm_year
			&& rtc_tm2.tm_hour == rtc_tm.tm_hour
			&& rtc_tm2.tm_min  == rtc_tm.tm_min
			&& rtc_tm2.tm_sec - rtc_tm.tm_sec == 5)			//add
//			&& rtc_tm2.tm_sec  == rtc_tm.tm_sec)			//add debug
	{
		fprintf(stderr, "Check OK!\n");
		close(fd);
		fclose(p_file);
    	return 0;	
	}
	else
	{
		fprintf(stderr, "Check Error!\n");
		/* if error, print the read value */
		fprintf( stderr, "%d-%d-%d, %02d:%02d:  %02d.\n",
    		rtc_tm2.tm_mday, rtc_tm2.tm_mon + 1, rtc_tm2.tm_year + 1900,
			rtc_tm2.tm_hour, rtc_tm2.tm_min, rtc_tm2.tm_sec);
    	
    	close(fd);
    	fclose(p_file);
       	return -1;
	}

    return 0;
}

