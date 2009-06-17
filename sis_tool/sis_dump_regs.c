#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

#define SISUSEDEVPORT
#define SIS_NEED_inSISREG
#define SIS_NEED_inSISREGW
#define SIS_NEED_inSISREGL
#define SIS_NEED_outSISREG
#define SIS_NEED_outSISREGW
#define SIS_NEED_outSISREGL
#define SIS_NEED_orSISREG
#define SIS_NEED_andSISREG
#define SIS_NEED_outSISIDXREG
#define SIS_NEED_inSISIDXREG
#define SIS_NEED_orSISIDXREG
#define SIS_NEED_andSISIDXREG
#define SIS_NEED_setSISIDXREG
#define SIS_NEED_setSISIDXREGmask

#define M_SISAR       SIS_IO_PORT_BASE + AROFFSET
#define M_SISARR      SIS_IO_PORT_BASE + ARROFFSET
#define M_SISGR       SIS_IO_PORT_BASE + GROFFSET
#define M_SISSR       SIS_IO_PORT_BASE + SROFFSET
#define M_SISCR       SIS_IO_PORT_BASE + CROFFSET
#define M_SISMISCR    SIS_IO_PORT_BASE + MISCROFFSET
#define M_SISMISCW    SIS_IO_PORT_BASE + MISCWOFFSET
#define M_SISINPSTAT  SIS_IO_PORT_BASE + INPUTSTATOFFSET
#define M_SISPART1    SIS_IO_PORT_BASE + PART1OFFSET
#define M_SISPART2    SIS_IO_PORT_BASE + PART2OFFSET
#define M_SISPART3    SIS_IO_PORT_BASE + PART3OFFSET
#define M_SISPART4    SIS_IO_PORT_BASE + PART4OFFSET
#define M_SISPART5    SIS_IO_PORT_BASE + PART5OFFSET
#define M_SISCAP      SIS_IO_PORT_BASE + CAPTUREOFFSET
#define M_SISVID      SIS_IO_PORT_BASE + VIDEOOFFSET
#define M_SISCOLIDXR  SIS_IO_PORT_BASE + COLREGOFFSET - 1
#define M_SISCOLIDX   SIS_IO_PORT_BASE + COLREGOFFSET
#define M_SISCOLDATA  SIS_IO_PORT_BASE + COLREGOFFSET + 1
#define M_SISCOL2IDX  SISPART5
#define M_SISCOL2DATA SISPART5 + 1

typedef unsigned long  ULong;
typedef unsigned short UShort;
typedef unsigned char  UChar;

#include "sis_regs.h"

#define SIS_IO_PORT_BASE 0x4800
#define SIS_IO_PORT_SPACE_LENGHT 128

struct sisregs;

static int cmd_read(int argc, char *argv[]);
static int cmd_write(int argc, char *argv[]);
static int cmd_exit(int argc, char *argv[]);
static void sis_unlock(struct sisregs *pregs);
static void sis_lock(struct sisregs *pregs);
static int sis_read_1(int custom, int idx);
static int sis_read_2(int custom, int idx);
static int sis_read_4(int custom, int idx);
static void sis_write_1(int custom, int idx, int value);
static void sis_write_2(int custom, int idx, int value);
static void sis_write_4(int custom, int idx, int value);

struct sisops {
    char name[50];
    int (*ops)(int argc, char *argv[]);
    char desc[200];
}sisops[] = {
    {"read", cmd_read, "read[/bhw] [sr,cr,part1] start"},
    {"write", cmd_write, "write[/bhw] [sr,cr,part1] start"},
    {"exit", cmd_exit, "exit"},
    {"none", NULL, ""},
};

typedef enum {
    TYPE_SR,
    TYPE_CR,
    TYPE_MISC,
    TYPE_PART1,
    TYPE_PART2,
    TYPE_PART3,
    TYPE_PART4,
    TYPE_NONE
}regtype;

struct sisregs {
    char name[20]; /* SR, CR, AR, VR, PART1 ... */
    int idx_base;
    int rw_base;
    int length;
    int width; /* always byte */
    regtype type;
    void (*unlock)(struct sisregs *);
    void (*lock)(struct sisregs *);
}sisregs[] = {
    {"sr", M_SISSR, M_SISSR + 1, 0x60, 1, TYPE_SR, sis_unlock, sis_lock},
    {"cr", M_SISCR, M_SISCR + 1, 0x7c, 1, TYPE_CR, sis_unlock, sis_lock},
    {"misc", M_SISMISCR, M_SISMISCW, 0x10, 1, TYPE_MISC, sis_unlock, sis_lock},
    {"part1", M_SISPART1, M_SISPART1 + 1, 0x60, 1, TYPE_PART1, sis_unlock, sis_lock},
    {"part2", M_SISPART2, M_SISPART2 + 1, 0xff, 1, TYPE_PART2, sis_unlock, sis_lock},
    {"part3", M_SISPART3, M_SISPART3 + 1, 0x3e, 1, TYPE_PART3, sis_unlock, sis_lock},
    {"part4", M_SISPART4, M_SISPART4 + 1, 0x3c, 1, TYPE_PART4, sis_unlock, sis_lock},
    {"none", 0, 0, 0, 0, TYPE_NONE, NULL, NULL},
};

struct sis_rw_ops {
    int (*read[4])(int custom, int idx);
    void (*write[4])(int custom, int base, int value);
    char fmt[4][20];
} sis_rw_ops = {
    {sis_read_1, sis_read_2, sis_read_2, sis_read_4},
    {sis_write_1, sis_write_2, sis_write_2, sis_write_4},
    {"%02x ", "%04x ", "%04x ", "%08x "},
};

int sisdevport;

/* read function */
static int sis_read_1(int custom, int idx)
{
    uint8_t val;
    if (custom >= 0) {
	if (idx > sisregs[custom].length)
	    return 0;
	else
	    inSISIDXREG(sisregs[custom].idx_base, idx, val);
    } else {
	val = inSISREG(idx);
    }
    return val;
}
static int sis_read_2(int custom, int idx)
{
    uint8_t val1;
    uint8_t val2;
    if (custom >= 0) {
	if (idx+1 > sisregs[custom].length) {
	    return 0;
	} else {
	    inSISIDXREG(sisregs[custom].idx_base, idx++, val1);
	    inSISIDXREG(sisregs[custom].idx_base, idx, val2);
	}
    } else {
	val1 = inSISREG(idx++);
	val2 = inSISREG(idx);
    }
    return (val2<<8) | val1;
}
static int sis_read_4(int custom, int idx)
{
    uint8_t val1;
    uint8_t val2;
    uint8_t val3;
    uint8_t val4;
    if (custom >= 0) {
	if (idx+3 > sisregs[custom].length) {
	    return 0;
	} else {
	    inSISIDXREG(sisregs[custom].idx_base, idx++, val1);
	    inSISIDXREG(sisregs[custom].idx_base, idx++, val2);
	    inSISIDXREG(sisregs[custom].idx_base, idx++, val3);
	    inSISIDXREG(sisregs[custom].idx_base, idx, val4);
	}
    } else {
	val1 = inSISREG(idx++);
	val2 = inSISREG(idx++);
	val3 = inSISREG(idx++);
	val4 = inSISREG(idx);
    }
    return (val4<<24) | (val3<<16) | (val2<<8) | val1;
}

/* write functions */
static void sis_write_1(int custom, int idx, int value)
{
    if (custom >= 0) {
	outSISIDXREG(sisregs[custom].idx_base, idx, (value & 0xff));
    } else {
	outSISREG(idx, (value & 0xff));
    }
}
static void sis_write_2(int custom, int idx, int value)
{
    if (custom >= 0) {
	if (idx+1 > sisregs[custom].length) {
	} else {
	    outSISIDXREG(sisregs[custom].idx_base, idx++, (value & 0xff));
	    outSISIDXREG(sisregs[custom].idx_base, idx, ((value>>8) & 0xff));
	}
    } else {
	outSISREG(idx++, (value & 0xff));
	outSISREG(idx, ((value>>8) & 0xff));
    }
}
static void sis_write_4(int custom, int idx, int value)
{
    if (custom >= 0) {
	if (idx+3 > sisregs[custom].length) {
	} else {
	    outSISIDXREG(sisregs[custom].idx_base, idx++, (value & 0xff));
	    outSISIDXREG(sisregs[custom].idx_base, idx++, ((value>>8) & 0xff));
	    outSISIDXREG(sisregs[custom].idx_base, idx++, ((value>>16) & 0xff));
	    outSISIDXREG(sisregs[custom].idx_base, idx, ((value>>24) & 0xff));
	}
    } else {
	outSISREG(idx++, (value & 0xff));
	outSISREG(idx++, ((value>>8) & 0xff));
	outSISREG(idx++, ((value>>16) & 0xff));
	outSISREG(idx, ((value>>24) & 0xff));
    }
}

static void sis_unlock(struct sisregs *pregs)
{
    int val;
    switch (pregs->type) {
	case TYPE_SR:
	    inSISIDXREG(pregs->idx_base, 0x05, val);
	    if (val != 0xa1) {
		outSISIDXREG(pregs->idx_base, 0x05, 0x86);
		/*inSISIDXREG(pregs->idx_base, 0x20, 0x20);*/
	    }
	    break;
	case TYPE_CR:
	    andSISIDXREG(pregs->idx_base, 0x11, 0x7f);
	    break;
	case TYPE_MISC:
	    break;
	case TYPE_PART1:
	    orSISIDXREG(pregs->idx_base, 0x2f, 0x01);
	    break;
	case TYPE_PART2:
	case TYPE_PART3:
	case TYPE_PART4:
	default:
	    break;
    }
}

static void sis_lock(struct sisregs *pregs)
{
    int val;
    switch (pregs->type) {
	case TYPE_SR:
	    inSISIDXREG(pregs->idx_base, 0x05, val);
	    if (val == 0xa1) {
		outSISIDXREG(pregs->idx_base, 0x05, 0x86);
	    }
	    break;
	case TYPE_CR:
	    orSISIDXREG(pregs->idx_base, 0x11, 0x80);
	    break;
	case TYPE_MISC:
	    break;
	case TYPE_PART1:
	    andSISIDXREG(pregs->idx_base, 0x2f, 0xfe);
	    break;
	case TYPE_PART2:
	case TYPE_PART3:
	case TYPE_PART4:
	default:
	    break;
    }
}

static int cmd_read(int argc, char *argv[])
{
    int offset;
    int count;
    int width;
    int start;
    int tmp;
    int custom;

    if (argc < 2) {
	return -1;
    }

    offset = strcspn(argv[0], "/");
    count = strtol(argv[0] + offset + 1, NULL, 0);
    switch (argv[0][strlen(argv[0])-1]) {
	case 'b':
	    width = 1;
	    break;
	case 'h':
	    width = 2;
	    break;
	case 'w':
	    width = 4;
	    break;
	default:
	    width = 4;
	    break;
    }

    /* if argv[1] is not begin with digit, then it will be the custom name of sis regs */
    if (argv[1][0] < '0' || argv[1][0] > '9') {
	for (custom = 0; sisregs[custom].type != TYPE_NONE; custom++) {
	    if (strcmp(argv[1], sisregs[custom].name) == 0) {
		break;
	    }
	}
	if (sisregs[custom].type == TYPE_NONE) {
	    printf("%s unrecognised\n", argv[1]);
	    return -2;
	}
	start = (argv[2] == 0 ? 0 : strtol(argv[2], NULL, 0));
	if (count == 0)
	    count = sisregs[custom].length/width;
	sisregs[custom].unlock(&sisregs[custom]);
    } else {
	custom = -1;
	start = strtol(argv[1], NULL, 0);
	if (count == 0)
	    count = 1;
    }

    tmp = 0;
    do {
	int i;
	int j;
	int t1 = (((count-(tmp*32/width))*width)>= 32)?(32/width):(count-(tmp*32)/width);
	if (custom >= 0) {
	    printf("%s[%02x]:\t", sisregs[custom].name, start+tmp*32);
	} else {
	    printf("%08x:\t", start+tmp*32);
	}
	for (i = 0; i < t1; i++) {
	    printf(sis_rw_ops.fmt[width-1], sis_rw_ops.read[width-1](custom, start+(tmp*32)+(i*width)));
	}
	printf("\n");
    } while(tmp++ < count*width/32);

    if (custom >= 0)
	sisregs[custom].lock(&sisregs[custom]);

    return 0;
}

static int cmd_write(int argc, char *argv[])
{
    int offset;
    int count;
    int width;
    int start;
    int value;
    int tmp;
    int custom;

    if (argc < 2) {
	return -1;
    }

    offset = strcspn(argv[0], "/");
    count = strtol(argv[0] + offset + 1, NULL, 0);
    if (count == 0)
	count = 1;
    switch (argv[0][strlen(argv[0])-1]) {
	case 'b':
	    width = 1;
	    break;
	case 'h':
	    width = 2;
	    break;
	case 'w':
	    width = 4;
	    break;
	default:
	    width = 4;
	    break;
    }

    /* if argv[1] is not begin with digit, then it will be the custom name of sis regs */
    if (argv[1][0] < '0' || argv[1][0] > '9') {
	for (custom = 0; sisregs[custom].type != TYPE_NONE; custom++) {
	    if (strcmp(argv[1], sisregs[custom].name) == 0) {
		break;
	    }
	}
	if (sisregs[custom].type == TYPE_NONE) {
	    printf("%s unrecognised\n", argv[1]);
	    return -2;
	}
	start = (argv[2] == 0 ? 0 : strtol(argv[2], NULL, 0));
	sisregs[custom].unlock(&sisregs[custom]);
    } else {
	custom = -1;
	start = strtol(argv[1], NULL, 0);
    }

    tmp = 0;
    do {
	int i;
	int j;
	char tmpbuf[200];
	int t1 = (((count-(tmp*32/width))*width)>= 32)?(32/width):(count-(tmp*32)/width);
	for (i = 0; i < t1; i++) {
	    sprintf(tmpbuf, sis_rw_ops.fmt[width-1], sis_rw_ops.read[width-1](custom, start+(tmp*32)+(i*width)));
retry:
	    if (custom >= 0) {
		printf("%s[%02x]:\t%s# ", sisregs[custom].name, start+tmp*32+(i*width), tmpbuf);
	    } else {
		printf("%08x:\t%s# ", start+tmp*32+(i*width), tmpbuf);
	    }
	    if (fscanf(stdin, "%x", &value) == 0)
		goto retry;
	    sis_rw_ops.write[width-1](custom, start+(tmp*32)+(i*width)+j, value);
	}
    } while(tmp++ < count*width/32);

    if (custom >= 0)
	sisregs[custom].lock(&sisregs[custom]);

    return 0;
}

static int cmd_exit(int argc, char *argv[])
{
    return 1;
}

static int parse_cmd(char *buf, struct sisops *ops)
{
    struct sisops *element;
    int argc = 0;
    int found = 0;
    int idx[100], count = 0;
    char (*argv[10])[20] = {NULL};
    char *p;
    int l1, l2;

    if (*buf == 0)
	return 0;

    /* generate argc and argv */
    p = buf;
    while (*p) {
	l1 = strspn(p, " ");
	p += l1;
	l2 = strcspn(p, " ");
	if (l2) {
	    argv[argc++] = strndup(p, l2);
	}
	p += l2;
    }

    for (element = ops; element->ops != NULL; element++, count++) {
	l1 = strcspn(argv[0], "/");
	if (!strncmp(element->name, argv[0], l1<strlen(element->name)?l1:strlen(element->name))) {
	    idx[found] = count;
	    found++;
	}
    }
    if (found == 1) {
	int ret = (ops[idx[0]].ops)(argc, argv); 
	if (ret < 0) {
	    printf("%s\n", ops[idx[0]].desc);
	} else if (ret > 0) {
	    return ret;
	}
    } else {
	for (count=0; count<found; count++) {
	    printf("\t%s", ops[idx[found]].name);
	}
    }
    return 0;
}

static int do_cmd()
{
    char buf[200];
    static int ret = 0;
    if (ret == 0) {
	fprintf(stderr, "sis_debug# ");
    } else {
	ret = 0;
    }
    fgets(buf, sizeof(buf), stdin);
    buf[strlen(buf)-1] = 0;
    if (buf[0] == 0) {
	ret = 1;
	return 0;
    }
    return parse_cmd(buf, sisops);
}

int main()
{
    if((sisdevport = open("/dev/port", O_RDWR, 0)) == -1) {
	printf("Failed to open /dev/port for read/write\n");
	goto myerror;
    }

    while (!do_cmd()); 

    return 0;
myerror:
    return -1;
}
