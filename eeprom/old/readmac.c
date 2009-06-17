#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h> 
#include<fcntl.h>
#include<linux/netlink.h>
#include<linux/rtnetlink.h>
#include<net/route.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define DEBUG

#define SIZE  0x10000
#define Cfg9346 0x50

/*The EEPROM commands include the alway-set leading bit.*/
#define EE_WEN_CMD      ( (4 << 6) | ( 3 << 4 ) )
#define EE_WRITE_CMD    (5 << 6)
#define EE_WDS_CMD      (4 << 6)
#define EE_READ_CMD     (6 << 6)
#define EE_ERASE_CMD    (7 << 6)

/*EEPROM_Ctrl bits.*/
#define EE_CS           0x08    /* EEPROM chip select. */
#define EE_SHIFT_CLK    0x04    /* EEPROM shift clock. */
#define EE_DATA_WRITE   0x02    /* EEPROM chip data in. */
#define EE_WRITE_0      0x00
#define EE_WRITE_1      0x02
#define EE_DATA_READ    0x01    /* EEPROM chip data out. */
#define EE_ENB          (0x80 | EE_CS)

#define inl(a)          (*(volatile unsigned int*)(a))
#define myoutb(v, a)    (*(volatile unsigned char *)(a) = (v))
#define inb(a)          (*(volatile unsigned char*)(a))
#define eeprom_delay()  inl(ee_addr)

int write_8169_eeprom(long, int, unsigned short);
int write_8169_eeprom8(long, int, unsigned short);

int gethex(int *vp, char *p, int n)
{               
	unsigned long v;       
	int digit;      

	for (v = 0; n > 0; n--) {
		if (*p == 0)    
			return (0);
		if (*p >= '0' && *p <= '9')
			digit = *p - '0';
		else if (*p >= 'a' && *p <= 'f')
			digit = *p - 'a' + 10;
		else if (*p >= 'A' && *p <= 'F')
			digit = *p - 'A' + 10;
		else
			return (0);

		v <<= 4;
		v |= digit; 
		p++;
	}               
	*vp = v;
	return (1);
}

static void write_eeprom_enable(long ioaddr){
	int i;
	long ee_addr = ioaddr + Cfg9346;
	int  cmd = EE_WEN_CMD;

	myoutb(EE_ENB & ~EE_CS, ee_addr);
	myoutb(EE_ENB, ee_addr);

	/* Shift the read command bits out. */
#if 1
	//16 bit
	for (i = 10; i >= 0; i--) {
#else
	for (i = 11; i >= 0; i--) {
#endif
		int dataval = (cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		myoutb(EE_ENB | dataval, ee_addr);
		eeprom_delay();
		myoutb(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
		eeprom_delay();
	}
	myoutb(~EE_CS, ee_addr);

}
/************************************************/
static void write_eeprom_disable(unsigned long ioaddr)
{
	int i;
	long ee_addr = ioaddr + Cfg9346;
	int  cmd = EE_WDS_CMD;

	myoutb(EE_ENB & ~EE_CS, ee_addr);
	myoutb(EE_ENB, ee_addr);

	/* Shift the read command bits out. */

#if 1
	i=10;  //16bit
#else   
	i=11;
#endif
	for (; i >= 0; i--) {
		int dataval = (cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		myoutb(EE_ENB | dataval, ee_addr);
		eeprom_delay();
		myoutb(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
		eeprom_delay();
	}
	myoutb(~EE_CS, ee_addr);
}
/************************************************/
int write_8169_eeprom(long ioaddr, int location,unsigned short data)
{
	int i;
	long ee_addr = ioaddr + Cfg9346;
	int  cmd = location | EE_WRITE_CMD;

	write_eeprom_enable(ioaddr);

	cmd <<= 16;
	cmd |= data;
	myoutb(EE_ENB & ~EE_CS, ee_addr);
	myoutb(EE_ENB, ee_addr);

	/* Shift the read command bits out. */
	for (i = 26; i >= 0; i--) {
		int dataval = (cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		myoutb(EE_ENB | dataval, ee_addr);
		eeprom_delay();
		myoutb(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
		eeprom_delay();
	}
	/* Terminate the EEPROM access. */
	myoutb(~EE_CS, ee_addr);
	eeprom_delay();
#if 0
	myoutb(EE_ENB, ee_addr);
	while( ! (inb(ee_addr) & EE_DATA_READ) ){
		myoutb(EE_ENB, ee_addr);
		eeprom_delay();
	}
#endif
#if 1
	write_eeprom_disable(ioaddr);
	/*delay()*/
	{
		volatile int j =0;
		while(j++<600000);
	}
#endif
	return 0;
}
/////
int write_8169_eeprom8(long ioaddr, int location,unsigned short data)
{
	int i;
	long ee_addr = ioaddr + Cfg9346;
	int  cmd = location | EE_WRITE_CMD;

	write_eeprom_enable(ioaddr);

	cmd <<= 8;
	cmd |= data;
	myoutb(EE_ENB & ~EE_CS, ee_addr);
	myoutb(EE_ENB, ee_addr);

	/* Shift the read command bits out. */
	for (i = 18; i >= 0; i--) {
		int dataval = (cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		myoutb(EE_ENB | dataval, ee_addr);
		eeprom_delay();
		myoutb(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
		eeprom_delay();
	}
	/* Terminate the EEPROM access. */

	myoutb(~EE_CS, ee_addr);
	eeprom_delay();

	myoutb(EE_ENB, ee_addr);

	while( ! (inb(ee_addr) & EE_DATA_READ) ){
		myoutb(EE_ENB, ee_addr);
		eeprom_delay();
	        }
	        return 0;
}
/************************************************************************/
int main (int ac, char *av[]){
	int fd;
	void *start, *start1;
	char *file = "/dev/mem";
	char rom;
	int i,j=0;
	unsigned short val = 0, v;
	
	FILE *fp;
	char buf[256];
	char *file1 = "/proc/bus/pci/00/07.0";
	int offset;

	fp = fopen(file1, "r+");
	if (fp == NULL){
		perror("fp ");
		printf("This file not open!!\n");
		return -1;
	}
#ifdef DEBUG
	printf("fp = 0x%08x\n", fp);
#endif
	fread(buf, sizeof(buf)/256, 18, fp);
	offset = *(unsigned int *)(&buf[16]);
	offset &= ~1;
	
	fd = open(file,O_RDWR);
#ifdef DEBUG
	printf("fd = %d\n",fd);
#endif
	start = mmap(NULL, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x1fd00000);
	if (start == NULL){
		printf("can't map!!\n");
		perror("mmap ");
		return 0;
	}
	start1 = start + offset;
#ifdef DEBUG
	printf("start = 0x%x, start1 = 0x%x\n", start, start1);
#endif
#ifdef __not_used 
	if (ac != 2){
		for (; start1 < start1 + 0x6; start1++){
			printf("%02x",*(volatile unsigned char *)start1);
			if (j == 5)
				printf("\n");
			else
				printf(":");
			j++;
		}
	}
#endif
	if(1) {
		unsigned short rom[] = {
				0x8129 ,0x10ec ,0x8139 ,0x10ec ,0x8139 ,0x4020 ,0xe512 ,0x0a00,
				0x56eb ,0x135b ,0x4d15 ,0xf7c2 ,0x8801 ,0x03b9 ,0x60f4 ,0x071a,
				0xdfa3 ,0x9836 ,0xdfa3 ,0x9836 ,0x03b9 ,0x60f4 ,0x1a1a ,0x1a1a,
				0x0000 ,0xb6e3 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x2000,
				0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000,
				0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000,
				0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000,
				0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000 ,0x0000};
		for(i=0; i< sizeof(rom)/2; i++)
			write_8169_eeprom(start1, i, rom[i]);

		myoutb(0x40&~EE_CS, start1 + Cfg9346);
		myoutb(0x40, start1 + Cfg9346);
		sleep(1);
		myoutb(0x00, start1 + Cfg9346);

	}
	for (i = 0; i < 3; i++) {
		val = 0;
		gethex(&v, av[1], 2);
		val = v ;
		av[1]+=3;

		gethex(&v, av[1], 2);
		val = val | (v << 8);
		av[1] += 3;
		
#if 1
		/*eeprom is 16 bit*/
		write_8169_eeprom(start1, 0x7 + i, val);
#else
		write_8169_eeprom8(start1, (0x7 +i) *2, val & 0xff);
		write_8169_eeprom8(start1, (0x7 +i) *2+1, (val >> 8) & 0xff);
#endif
	}
	munmap(start,SIZE);
	fclose(fp);
	close(fd);
	printf("The machine should be reboot to make the MAC change to take effect!!\n");
	return 0;
} 

