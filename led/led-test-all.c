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


int write_step = 0;
struct ecreg {
	unsigned long addr;
	unsigned char  val;
};
struct ecreg ecreg;


int main(int argc, char *argv[])
{
	int fd;
	int select = 0;

	if (argc < 2) {
		fprintf(stderr, "args not enough.\n");
		return -1;
	}
	if (argc > 2) {
		fprintf(stderr, "args too more.\n");
		return -1;
	}
	
	fd = open(EC_MISC_DEV, O_RDWR);
	if(fd < 0){
		fprintf(stderr, "open error.\n");
		return -1;
	}


#define  GPIOBASE	0xf400
#define  MODE_SWITCHER (GPIOBASE+0xc2)
#define  REG_LED	(GPIOBASE+0xc8)

#define  POWER_BLUE_BIT	(1<<0)
#define  POWER_ORANGE_BIT	(1<<1)
#define  CHARGE_GREEN_BIT	(1<<2)
#define  CHARGE_RED_BIT	(1<<3)
#define  NUM_LOCK_BIT	(1<<4)
#define  CAPS_LOCK_BIT	(1<<5)

	
	// Entering LED test mode
	ecreg.addr = MODE_SWITCHER;
	ioctl(fd, IOCTL_READ_REG, &ecreg);
	ecreg.val = 0x01;
	ioctl(fd, IOCTL_WRITE_REG, &ecreg);

#if 1
	switch (argv[1][0]) {
	
	case '0':
	// Leave LED test mode
		ecreg.addr = MODE_SWITCHER;
		ioctl(fd, IOCTL_READ_REG, &ecreg);
		ecreg.val = 0x00;
		ioctl(fd, IOCTL_WRITE_REG, &ecreg);
		break;
	/* Light LED */
	case '3':
		ecreg.addr = REG_LED;
		ioctl(fd, IOCTL_READ_REG, &ecreg);
		ecreg.val |= (POWER_BLUE_BIT | POWER_ORANGE_BIT | CHARGE_GREEN_BIT | CHARGE_RED_BIT | NUM_LOCK_BIT | CAPS_LOCK_BIT);
		ioctl(fd, IOCTL_WRITE_REG, &ecreg);
		break;
	/* Shutoff LED */
	case '7':
		ecreg.addr = REG_LED;
		ioctl(fd, IOCTL_READ_REG, &ecreg);
		ecreg.val &= ~(POWER_BLUE_BIT | POWER_ORANGE_BIT | CHARGE_GREEN_BIT | CHARGE_RED_BIT | NUM_LOCK_BIT | CAPS_LOCK_BIT);
		ioctl(fd, IOCTL_WRITE_REG, &ecreg);
		break;
	}
#endif

	// Leave LED test mode
	//ecreg.addr = MODE_SWITCHER;
	//ioctl(fd, IOCTL_READ_REG, &ecreg);
	//ecreg.val = 0x00;
	//ioctl(fd, IOCTL_WRITE_REG, &ecreg);

	close(fd);
	return 0;
}
