/* 
 * Control network occupation
 *
 * Copyright (C) 2008 by Cuicb <cuicb@lemote.com> lemote Inc
 *
 * Licensed under GPLv2 or later
 *
 */

#include <libnet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>

#define ETHERTYPE_NULL 0xffff
#define PACK 1010


void spin(int);

typedef struct pkt {
	struct ether_header ether_part;
	char padding[PACK];
}pkt;

int fill_pkt(char *dev, struct pkt * packet)
{
	int skfd, i;
	struct ifreq ifr;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return -1;
	}
	bzero(&ifr, sizeof(ifr));
	strcpy(ifr.ifr_name, dev);

	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		perror("ioctl");
		return -1;
	}
	close(skfd);
#if 0
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
			(unsigned char) ifr.ifr_hwaddr.sa_data[0],
			(unsigned char) ifr.ifr_hwaddr.sa_data[1],
            (unsigned char) ifr.ifr_hwaddr.sa_data[2],
            (unsigned char) ifr.ifr_hwaddr.sa_data[3],
            (unsigned char) ifr.ifr_hwaddr.sa_data[4],
            (unsigned char) ifr.ifr_hwaddr.sa_data[5]);
#endif
	for (i=0; i<6; i++) {
		packet->ether_part.ether_shost[i] =
		packet->ether_part.ether_dhost[i] = (unsigned char) ifr.ifr_hwaddr.sa_data[i];
		
	}

	packet->ether_part.ether_type = ETHERTYPE_NULL;
	bzero(packet->padding, PACK);

	return 0;
}

int sendpkt(char *dev, unsigned char *packet, unsigned size, unsigned freq )
{
	char errbuf[1024];
	struct timeval cur_time, pre_time;
	unsigned long time_left, time_elapse, count, nsend;
	libnet_t *handle;

	count = 0;
	handle = libnet_init(LIBNET_LINK, dev, errbuf);
	while (1) {
		gettimeofday( &pre_time, NULL);
		if ((nsend = libnet_write_link(handle, packet, size)) == -1) {
			fprintf(stderr, "packet send failure: %s\n", errbuf);
			return 1;
		}
		gettimeofday( &cur_time, NULL);
		time_elapse = (cur_time.tv_sec - pre_time.tv_sec) * 1000 + 
			cur_time.tv_usec - pre_time.tv_usec;
		time_left =  freq > time_elapse ? freq - time_elapse : 0;
		
		count ++;
		spin(count);
//		printf("%ldth pkt, write %ld bytes, time cost: %d, will sleep %d\n", count, nsend, time_elapse, time_left);
		usleep(time_left);
	}

	libnet_destroy(handle);
	return 0;
}


int main(int argc, char *argv[] )
{
	if (argc != 4) {
		printf("usage: %s interface mode ratio\n", argv[0]);
		return 0;
	}

	struct pkt packet;
	unsigned long interval;

	interval =(unsigned long) (1E8 / (float) (atoi(argv[2]) * atoi(argv[3]) * 128));
//	printf("%ld\n", interval);
	
	fill_pkt(argv[1], &packet);
	sendpkt(argv[1], (char *)&packet, sizeof(packet), interval);
	
	return 0;
}

void spin(int count)
{
	switch (count % 8)
	{
		case 0: fprintf(stderr, "\b|");break;
		case 1: fprintf(stderr, "\b/");break;
		case 2: fprintf(stderr, "\b-");break;
		case 3: fprintf(stderr, "\b\\");break;
		case 4: fprintf(stderr, "\b|");break;
		case 5: fprintf(stderr, "\b/");break;
		case 6: fprintf(stderr, "\b-");break;
		case 7: fprintf(stderr, "\b\\");break;
		default: break;
	}
}
