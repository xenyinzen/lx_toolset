/* 
 * Control CPU occupation
 *
 * Copyright (C) 2008 by Cuicb <cuicb@lemote.com> lemote Inc
 *
 * Licensed under GPLv2 or later
 *
 */


#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

#define IDLE 10000
#define SAMPLE 300000
#define HZ 10000
#define CORE_NUM 1

float defaults = 0;

typedef struct jiffy_counts_t {
	unsigned long long usr,nic,sys,idle,iowait,irq,softirq,steal;
	unsigned long long total;
	unsigned long long busy;
} jiffy_counts_t;


void get_jiffy_counts(struct jiffy_counts_t * jif)
{
	FILE* fp = fopen("/proc/stat", "r");
	if (fscanf(fp, "cpu  %lld %lld %lld %lld %lld %lld %lld %lld",
			 &jif->usr, &jif->nic, &jif->sys, &jif->idle,
			 &jif->iowait, &jif->irq, &jif->softirq, &jif->steal) < 4) {
			printf("failed to read /proc/stat");
	}
	fclose(fp);
	jif->total = jif->usr + jif->nic + jif->sys + jif->idle
				 + jif->iowait + jif->irq + jif->softirq + jif->steal;
	/* procps 2->x does not count iowait as busy time */
	jif->busy = jif->total - jif->idle - jif->iowait;
}

float get_ratio(float user_ratio)
{
	float ratio, tmp;
	struct jiffy_counts_t pre_jif, jif;
	printf("set user_ratio: %4f%\n", user_ratio*100);
	
	while (1) {
	get_jiffy_counts(&pre_jif);
	usleep(SAMPLE);
	get_jiffy_counts(&jif);
	ratio = CORE_NUM * (float)(jif.busy - pre_jif.busy) / 
			(float)(jif.total - pre_jif.total);
//	printf("current: %f\n", ratio);
	tmp = user_ratio  - ratio + defaults ;	
//	defaults = defaults > 0 && defaults < 1 ? defaults : user_ratio;
	if (tmp < 1 && tmp > 0 ) defaults = tmp;
//	printf("defaults: %f\n", defaults);
	}
	return ratio;
}

int set_ratio()
{
	unsigned i, count;
	struct timeval cur_time, pre_time;
	unsigned jiffy;

	// cacalate 1000 loop time cost
	gettimeofday( &pre_time, NULL);
	for (i=0; i<HZ; i++) ; 
	gettimeofday( &cur_time, NULL);
	
	jiffy = (cur_time.tv_sec - pre_time.tv_sec) * 1000000 + 
			cur_time.tv_usec - pre_time.tv_usec;

	//printf("time jiffy : %d\n", jiffy);
	while (1) {
	//	printf("setting ratio : %f\n", defaults);
	// caculate how many loops needs
	count = HZ * (float) (defaults * IDLE) /
			(float)((1 - defaults) * jiffy);
	//	printf("count:%d\n", count);
	
	
	for (i=0; i<count; i++) ;	// busy loop
	usleep(IDLE);
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("usage: %s ratio\n", argv[0]);
		return 0;
	}
	float user_ratio;
	user_ratio = (float)atoi(argv[1]) / 100;
	pthread_t id;
	if (pthread_create(&id, NULL, (void *)set_ratio, NULL) != 0) return -1;
	//sleep(2);
	get_ratio(user_ratio);
	return 0;
}
