#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

#define DELAY	10000		/* 100ms */
#define BUFSIZE	1024*1024		/* 1k buffer */

void delay()
{
	usleep(DELAY);
}


int main(int argc, char *argv[])
{
	int sfd, tfd, nulfd, pid;
	struct timeval cur_time, pre_time;
	unsigned long jiffy, size, idle;
	char *buf;


	if (argc != 3) {
		printf("usage: %s dev speed\n", argv[0]);
		return 0;
	}

	if ((sfd = open("/dev/urandom", O_RDONLY)) == -1) {
		perror("open:");
		return 1;
	}

	if ((tfd = open(argv[1], O_WRONLY | O_CREAT)) == -1) {
		perror("open:");
		return 1;
	}

	if ((nulfd = open("/dev/null", O_WRONLY)) == -1) {
		perror("open:");
		return 1;
	}

	signal(SIGALRM, delay);
	idle = atol(argv[2]);
	buf = (char *)malloc(BUFSIZE);
	
	printf("disk io speed: %f bytes/s\n", (float) idle/(float)(idle +DELAY) * 55);

	ualarm(idle, idle);
	while (1)
	{
		read(sfd, buf, BUFSIZE);
		write(tfd, buf, BUFSIZE);
	
		read(tfd, buf, BUFSIZE);
		write(nulfd, buf, BUFSIZE);
	
		fsync(tfd);
	}

	free(buf);
	close(sfd);
	close(tfd);
	close(nulfd);
	return 0;
}

