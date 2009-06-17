/*
 * short test file for kb3310
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/types.h>
#include <linux/ioctl.h>

/*******************************************************/

// device of ec misc
#define	EC_MISC_DEV		"/dev/ec_misc"
// ioctl for read write register and program EC or IE(information element)
#define	EC_IOC_MAGIC		'E'
/* misc ioctl operations */
#define	IOCTL_READ_REG		_IOR(EC_IOC_MAGIC, 1, int)
#define	IOCTL_WRITE_REG		_IOW(EC_IOC_MAGIC, 2, int)

struct ecreg {
	unsigned long addr;
	unsigned char  val;
};
struct ecreg ecreg;

/**
 * filename [r|w] [1-4] [0|1]
 * 
 **/
int main(int argc, char *argv[])
{
	int fd;
	int select = 0;
	int addr;

	//if (argc < 4) {
	if (argc < 2) {
		fprintf(stderr, "args not enough\n");
		return -1;
	}
	if (argc > 3) {
		fprintf(stderr, "too many args\n");
		return -1;
	}

	addr = strtol(argv[1], NULL, 16);
	if(addr > 0x10000){
		fprintf(stderr, "ecreg : addr not available\n");
		return -1;
	}

	fd = open(EC_MISC_DEV, O_RDWR);
	if(fd < 0){
		fprintf(stderr, "open error.\n");
		return -1;
	}

#if 0
#define  GPIOBASE		0xf400
#define  MUTE_SWITCH	0xe7		// index 1, read & write
#define  CRT_DETECT		0xad		// index 2, readonly
#define  LID_STATUS		0xbd		// index 3, readonly
#define  BEEP_FLAG		0xd0		// index 4, read & write
#endif

	if (argc == 2){
		ecreg.addr = addr;
		ioctl(fd, IOCTL_READ_REG, &ecreg);
		printf("Read EC reg, addr : 0x%x, value : 0x%x\n", ecreg.addr, ecreg.val);
	}
	
	if (argc == 3){
		ecreg.addr = addr;
		ioctl(fd, IOCTL_READ_REG, &ecreg);
		printf("Before writing, ecreg : addr=0x%x, value=0x%x\n", ecreg.addr, ecreg.val);
		ecreg.val = atoi(argv[2]);
		ioctl(fd, IOCTL_WRITE_REG, &ecreg);
		ioctl(fd, IOCTL_READ_REG, &ecreg);
		printf("After writing, ecreg : addr=0x%x, value=0x%x\n", ecreg.addr, ecreg.val);

	}

	close(fd);
	return 0;
}

