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
#define	IOCTL_READ_EC		_IOR(EC_IOC_MAGIC, 3, int)
#define	IOCTL_PROGRAM_IE	_IOW(EC_IOC_MAGIC, 4, int)
#define	IOCTL_PROGRAM_EC	_IOW(EC_IOC_MAGIC, 5, int)

#define WRITE_STEP		0x10000
#define	IE_START_ADDRESS	(0x20000 + write_step)

int write_step = 0;
struct ecreg {
	unsigned long addr;
	unsigned char  val;
};
struct ecreg ecreg;


int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	unsigned char *buf;
	int i;
	unsigned int size;
	struct stat st;
	FILE *fp;

	if (argc < 3)
		return -1;
	fd = open(EC_MISC_DEV, O_RDWR);
	if(fd < 0){
		printf("open error.\n");
		return -1;
	}

	if (strcmp(argv[2], "smt") == 0) {
		write_step  = 0*WRITE_STEP;
	} else if (strcmp(argv[2], "fat") == 0) {
		write_step  = 1*WRITE_STEP;
	} else if (strcmp(argv[2], "frt") == 0) {
		write_step  = 2*WRITE_STEP;
	} else if (strcmp(argv[2], "fft") == 0) {
		write_step  = 3*WRITE_STEP;
	}

//	printf("Get from SPI Flash\n");	
	
	ecreg.addr = IE_START_ADDRESS;
	ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
	size = ecreg.val;
	ecreg.addr = IE_START_ADDRESS + 1;
	ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
	size = (ecreg.val << 8) | size;
	ecreg.addr = IE_START_ADDRESS + 2;
	ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
	size = (ecreg.val << 16) | size;
	ecreg.addr = IE_START_ADDRESS + 3;
	ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
	size = (ecreg.val << 24) | size;
	buf = malloc(size);

	fp = fopen(argv[1], "w");
	if (fp == NULL)
		return -1;
//	printf("size = %d\n", size);
	for (i=0;i<size;i++) {
		ecreg.addr = IE_START_ADDRESS + 4 + i;
		ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
		buf[i] = ecreg.val;
		fprintf(fp, "%c", buf[i]);
	}

	close(fd);
	fclose(fp);
	free(buf);
	buf = NULL;
	return 0;
}
