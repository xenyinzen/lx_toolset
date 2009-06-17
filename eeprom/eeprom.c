#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h> 
#include <fcntl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/route.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* ---------------------------------------- */
/*             MICRO SWITCHES               */
/* ---------------------------------------- */
/* read */
#define READ_ 			0
/* write */
#define WRITE_ 			1
/* READ_ and WRITE_ are conflicting MICROS */

/* clear eeprom */
#define EREASE_EEPROM 	0
/* mac or rom? */
#define MAC_  			0
/* EREASE_EEPROM and MAC_ are conflicting MICROS */

/* 8bits or 16bits? */
#define BITS16  			1

/* Netcard Type  
 * 1: 8139
 * 2: 8169 or 8110
 * 3: etc
 */
#define NETCARD_TYPE	2
/* ---------------------------------------- */


/* #define EEPROM_DEBUG */

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

#define write(val, ad)  (*(volatile unsigned char *)ad) = val
#define RTL_W8(address, reg, val)   write(val, (address + reg))

int write_eeprom(long, int, unsigned short);
int write_eeprom8(long, int, unsigned short);
static int read_eeprom(int, int);

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
#if BITS16 == 1
	//16 bit
	for (i = 10; i >= 0; i--) {
#endif

#if BITS16 == 0
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

#if BITS16 == 1
	i=10;  //16bit
#endif

#if BITS16 == 0
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
int write_eeprom(long ioaddr, int location,unsigned short data)
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

	write_eeprom_disable(ioaddr);
	/*delay()*/
	{
		volatile int j =0;
		while(j++<600000);
	}

	return 0;
}

int write_eeprom8(long ioaddr, int location,unsigned short data)
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

/*   */
static int read_eeprom(int ioaddr, int location) {
	int i;
	unsigned int retval = 0;
	int ee_addr = ioaddr + Cfg9346;
	int read_cmd = location | EE_READ_CMD;

	RTL_W8(ioaddr, Cfg9346, EE_ENB & ~EE_CS);
	RTL_W8(ioaddr, Cfg9346, EE_ENB );

#if BITS16 == 1   /* 16 bit */ 
	/* Shift the read command bits out. */
	for (i = 10; i >= 0; i--) {
		int dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		RTL_W8(ioaddr, Cfg9346, EE_ENB | dataval);
		eeprom_delay();
		RTL_W8(ioaddr, Cfg9346, EE_ENB | dataval | EE_SHIFT_CLK);
		eeprom_delay();
	}
#endif

#if BITS16 == 0
  /* 8 bit */
	for (i = 11; i >= 0; i--) {
		int dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		RTL_W8(ioaddr, Cfg9346, EE_ENB | dataval);
		eeprom_delay();
		RTL_W8(ioaddr, Cfg9346, EE_ENB | dataval | EE_SHIFT_CLK);
		eeprom_delay();
	}
#endif

	RTL_W8(ioaddr, Cfg9346, EE_ENB);
	eeprom_delay();

#if BITS16 == 1
   /* 16 bit */
	for (i = 16; i > 0; i--) {
		RTL_W8(ioaddr, Cfg9346, EE_ENB | EE_SHIFT_CLK);
		eeprom_delay();
		retval = (retval << 1) | ((inb(ee_addr) & EE_DATA_READ) ? 1 : 0);
		RTL_W8(ioaddr, Cfg9346, EE_ENB);
		eeprom_delay();
	}
	RTL_W8(ioaddr,~EE_CS, ee_addr);
#endif

#if BITS16 == 0
   /* 8 bit */
	for (i = 8; i > 0; i--) {
		RTL_W8(ioaddr, Cfg9346, EE_ENB | EE_SHIFT_CLK);
		eeprom_delay();
		retval = (retval << 1) | ((inb(ee_addr) & EE_DATA_READ) ? 1 : 0);
		RTL_W8(ioaddr, Cfg9346, EE_ENB);
		eeprom_delay();
	}
	RTL_W8(ioaddr,~EE_CS, ee_addr);   //END
#endif

	/* Terminate the EEPROM access. */
	return retval;
}

/* main function */
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
	//char *file1 = "/proc/bus/pci/00/06.0";
	int offset;

	fp = fopen(file1, "r+");
	if (fp == NULL){
		perror("fp ");
		printf("This file not open!!\n");
		return -1;
	}
#ifdef EEPROM_DEBUG
	printf("fp = 0x%08x\n", fp);
#endif
	fread(buf, sizeof(buf)/256, 18, fp);
	offset = *(unsigned int *)(&buf[16]);
	offset &= ~1;
	
	fd = open(file,O_RDWR);

#ifdef EEPROM_DEBUG
	printf("fd = %d\n",fd);
#endif

	/* start is net card device map address memory space */
	start = mmap(NULL, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x1fd00000);
	if (start == NULL){
		printf("can't map!!\n");
		perror("mmap ");
		return 0;
	}
	/* start1 is net card io space map address of memory space */
	/* offset is io space skewing */
	start1 = start + offset;

#ifdef EEPROM_DEBUG
	printf("start = 0x%x, start1 = 0x%x\n", start, start1);
#endif

#if READ_ == 1 &&  MAC_ == 1
	if (ac != 2) {
		for (; start1 < start + offset + 0x6; start1++) {
			printf( "%02x",*(volatile unsigned char *)start1 );
			if (j == 5)
				printf("\n");
			else
				printf(":");
			j++;
		}
		return 1;
	}
#endif

/* read eeprom */
#if READ_ == 1 && MAC_ == 0

	for (i = 0; i < 64; i++){
		int dat;
		
		dat = read_eeprom(start1, i);
//		printf("%02x:%02x", dat&0xff, (dat & 0xff00) >> 8);
		printf ("0x%04x", dat);
	
	if ( i == 7 || i == 15 || i == 23 || i == 31 
	     || i == 39 || i == 47 || i == 55 || i == 63) 
		{
			printf("\n");
		}
		else
			printf(", ");
			
	}
	return 1;
#endif

/* ------------------------------------ WRITE -------------------------------------- */

#if WRITE_ == 1 && EREASE_EEPROM == 1
	/* clear eeprom */
		unsigned short eeprom_dat[]=
		{
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
		};
		for (i = 0; i < sizeof(eeprom_dat)/2; i++){
			write_eeprom(start1, i, eeprom_dat[i]);
		}
#endif

#if WRITE_ == 1 && MAC_ == 1
	if (ac == 1){
		printf("This command format:  ");
		printf("./cmd  xx:xx:xx:xx:xx:xx\n");
		return 0;
	}
	for (i = 0; i < 3; i++) {
		val = 0;
		gethex(&v, av[1], 2);
		val = v ;
		av[1]+=3;

		gethex(&v, av[1], 2);
		val = val | (v << 8);
		av[1] += 3;
		
#if BITS16 == 1
		/*eeprom is 16 bit*/
		write_eeprom(start1, 0x7 + i, val);
#endif
#if BITS16 == 0
		write_eeprom8(start1, (0x7 +i) *2, val & 0xff);
		write_eeprom8(start1, (0x7 +i) *2+1, (val >> 8) & 0xff);
#endif
	}
#endif


#if WRITE_ == 1 && MAC_ == 0 && NETCARD_TYPE == 1
	/***write 8139 eeprom***/
	unsigned short eeprom_data[]=
		{
			0x8129, 0x10ec, 0x8139, 0x10ec, 0x8139, 0x4020, 0xe112, 0xe000,
			0x014c, 0x0603, 0x4d15, 0xf7c2, 0x8801, 0x43b9, 0xb0f2, 0x031a,
			0xdf43, 0x8a36, 0xdf43, 0x8a36, 0x43b9, 0xb0f2, 0x1111, 0x1111,
			0x0000, 0x7ed7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2000,
			0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
			0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
			for (i = 0; i < sizeof(eeprom_data)/2; i++){
				write_eeprom(start1, i, eeprom_data[i]);
			}
#endif

#if WRITE_ == 1 && MAC_ == 0 && NETCARD_TYPE == 2
	/***write 8169 eeprom***/
		unsigned short eeprom_data[]=
		{
			0x8129, 0x10ec, 0x8169, 0x10ec, 0x8169, 0x4020, 0xa101, 0xe000,
			0x674c, 0x0022, 0xcd15, 0xf7c2, 0x8000, 0x0000, 0x0000, 0x1300,
			0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
			0x0000, 0x7ed7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2000,
			0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
			0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
			0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
			0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 
		};
		
		for (i = 0; i < sizeof(eeprom_data)/2; i++){
			write_eeprom(start1, i, eeprom_data[i]);
		}
#endif

	munmap(start,SIZE);
	fclose(fp);
	close(fd);
	printf("The machine should be reboot to make the EEPROM rom change to take effect!!\n");
	return 0;
} 

