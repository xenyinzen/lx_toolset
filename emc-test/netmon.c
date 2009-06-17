/* 
 * Monitor network occupation
 *
 * Copyright (C) 2008 by Cuicb <cuicb@lemote.com> lemote Inc
 *
 * Licensed under GPLv2 or later
 *
 */

#include <stdio.h>
#include <pcap.h>
#include <unistd.h>
#include <netinet/in.h>
#define SIZE_ETHERNET 14
#define ETHER_ADDR_LEN 6
//Ethernet header

unsigned count = 0;
struct timeval time_new, time_old;

struct sniff_ethernet
{
  u_char ether_dhost[ETHER_ADDR_LEN];
  u_char ether_shost[ETHER_ADDR_LEN];
  u_short ether_type;
};

void get_packet(u_char *args,const struct pcap_pkthdr *header,const u_char *packet)
{
  const struct sniff_ethernet *ethernet;
  const struct sniff_ip *ip;
  const struct sniff_tcp *tcp;
  unsigned long interval;

  
  u_int size_ip;
  u_int size_tcp;
  
  ethernet = (struct sniff_ethernet *)(packet);
  if (ethernet->ether_type == 0xffff)  {
	  count++;
	  count = count % 100;
//	  printf(".%d.\n", count);
	  if (count == 0) {
		  gettimeofday( &time_new, NULL);
		  interval = (time_new.tv_sec - time_old.tv_sec) * 1000000 + 
			  		  time_new.tv_usec - time_old.tv_usec;
		  
		  time_old.tv_sec = time_new.tv_sec;
		  time_old.tv_usec = time_new.tv_usec;
		  fprintf(stderr, "\rcurrent flow rate: %ldk bytes/sec",(long)((float)100*1000000/(float)interval));
	  }
  }
}

int main(int argc,char *argv[])
{
  char errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program fp;
  char filter_exp[]="\0";
  bpf_u_int32 mask;
  bpf_u_int32 net;
  pcap_t *handle;
  struct pcap_pkthdr header;
  const u_char *packet;
 
  if (argc != 2) {
	  printf("usage: %s interface\n", argv[0]);
	  return 0;
  }
  printf("Listening on :%s\n", argv[1]);
  //search ip address and netmask
  if (pcap_lookupnet(argv[1], &net, &mask, errbuf) == -1)
  {
    fprintf(stderr,"can't get netmask for device%s\n", argv[1]);
    net = 0;
    mask = 0;
  }
  //open device
  handle = pcap_open_live (argv[1], BUFSIZ, 1, 1000, errbuf);
  if (handle == NULL)
  {
    fprintf(stderr,"can't open device%s\n", argv[1], errbuf);
    return 3;
  }
  //compile filter
  if (pcap_compile(handle,&fp,filter_exp,0,net) == -1)
  {
    fprintf(stderr,"can't parse filter %s:%s\n",filter_exp,pcap_geterr(handle));
    return 4;
  }
  //set filter
  if (pcap_setfilter(handle,&fp) == -1)
  {
    fprintf(stderr,"can't install filter %s:%s",filter_exp,pcap_geterr(handle));
    return 5;
  }
  //grab a packet
  pcap_loop(handle,-1,get_packet,NULL);
  //  packet = pcap_next(handle, &header);
  //print its length
  //  printf("jacked a packet with length of [%d]\n",header.len);
  pcap_close(handle);
  return 0;
}
