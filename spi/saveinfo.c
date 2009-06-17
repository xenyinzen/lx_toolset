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

struct ecreg {
	unsigned long addr;
	unsigned char  val;
};
struct ecreg ecreg;
int write_step = 0;

#define	UL_SIZE		(sizeof(unsigned long))

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	unsigned char *buf;
	int log_fd;
	int i;
	int size;
	struct stat st;
	unsigned long offset;

	if(argc < 3){
		printf("%s file\n", argv[0]);
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

	if ((log_fd = open(argv[1], O_RDONLY)) == -1) {
		printf("%s not found\n", argv[1]);
		return -2;
	}

	stat(argv[1], &st);
	if (st.st_size == 0) {
		return 0;
	}
#if 0
	printf("file size %d\n", st.st_size);
#endif

	buf = malloc(st.st_size + 3 * UL_SIZE);
	*(unsigned long *)buf = write_step;
	*(unsigned long *)(buf + 4) = st.st_size + UL_SIZE;
	*((unsigned long *)(buf + 8)) = st.st_size;
	read(log_fd, buf + 3 * UL_SIZE, st.st_size);

	fd = open(EC_MISC_DEV, O_RDWR);
	if(fd < 0){
		printf("open error.\n");
		return 1;
	}

	ret = ioctl(fd, IOCTL_PROGRAM_IE, buf);
	if(ret < 0){
		printf("program ie : ioctl error.\n");
		return 2;
	}

//	printf("Get from SPI Flash\n");	
	ecreg.addr = IE_START_ADDRESS;
	ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
	size = ecreg.val;
	ecreg.addr = IE_START_ADDRESS + 1;
	ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
	size = size | (ecreg.val << 8);
	ecreg.addr = IE_START_ADDRESS + 2;
	ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
	size = size | (ecreg.val << 16);
	ecreg.addr = IE_START_ADDRESS + 3;
	ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
	size = size | (ecreg.val << 24);

#if 0
	printf("program size: %d\n", size);
	
	for (i=0;i<st.st_size;i++) {
		ecreg.addr = 0x20004 + i;
		ret = ioctl(fd, IOCTL_READ_EC, &ecreg);
		printf("%c", ecreg.val);	
	}
	printf("\n");
#endif

	close(fd);
	close(log_fd);
	free(buf);
	buf = NULL;
	return 0;
}
