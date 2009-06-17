/***************************************************************************
 * Running Unix Memory Test
 * (C) Nicolas George - 2003
 * rumt_trymem: main program
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "rumt.h"

#ifndef DEV_MEM
/* it may fail here, path.h is not SUS, but at least Linux, OpenBSD and
 * FreeBSD have it. Not Solaris. */
# include <paths.h>
# ifdef _PATH_MEM
#  define DEV_MEM _PATH_MEM
# endif
#endif
#ifndef DEV_MEM
# define DEV_MEM NULL
#endif

typedef struct memory_area_t {
    unsigned offset;
    unsigned n_pages;
} memory_area_t;

const char *dev_mem = DEV_MEM;
const char *input_seed = NULL;
const char *output_seed = NULL;

long page_size;
int device;

static memory_area_t *areas;
static int n_areas;
static unsigned char *buffer;

static void
usage(FILE *f)
{
    fprintf(f, "Usage: rumt_trymem [-h] [-d dev] [-i seed] [-o seed] area [area...]\n");
}

static int
number(const char **t)
{
    char *e;
    int r;
    int u = 0;

    r = strtol(*t, &e, 0);
    if(e == *t) {
	*t = NULL;
	return(0);
    }
    switch(*e) {
	case 'b': case 'B':
	    u = 1;
	    break;
	case 'k': case 'K':
	    u = 1024;
	    break;
	case 'm': case 'M':
	    u = 1024 * 1024;
	    break;
	case 'p': case 'P':
	    u = page_size;
	    break;
    }
    if(u > 0) {
	r *= u;
	e++;
    }
    *t = e;
    return(r);
}

static void
check_areas(const unsigned char *seed, long seed_length)
{
    unsigned char *buffer2, x;
    int i, j, k, b;
    unsigned long o;

    buffer2 = buffer + page_size;
    md5stream_init();
    md5stream_seed(seed, seed_length);
    for(i = 0; i < n_areas; i++) {
	o = areas[i].offset;
	for(j = areas[i].n_pages; j > 0; j--, o += page_size) {
	    md5stream_fill(buffer, page_size);
	    if(lseek(device, o, SEEK_SET) != o) {
		perror("lseek");
		exit(1);
	    }
	    if(read(device, buffer2, page_size) != page_size) {
		perror("read");
		exit(1);
	    }
	    for(k = 0; k < page_size; k++) {
		x = buffer[k] ^ buffer2[k];
		if(x) {
		    for(b = 0; b < 7; b++) {
			if(((x >> b) & 1) != 0) {
			    printf("0x%12.12lX.%d%c\n",
				o + k, b,
				((buffer2[k] >> b) & 1) != 0 ? '+' : '-');
			    fflush(stdout);
			}
		    }
		}
	    }
	}
    }
}

static void
write_areas(const unsigned char *seed, long seed_length)
{
    int i, j;
    unsigned long o;

    md5stream_init();
    md5stream_seed(seed, seed_length);
    for(i = 0; i < n_areas; i++) {
	o = areas[i].offset;
	for(j = areas[i].n_pages; j > 0; j--, o += page_size) {
	    md5stream_fill(buffer, page_size);
	    if(lseek(device, o, SEEK_SET) != o) {
		perror("lseek");
		exit(1);
	    }
	    if(write(device, buffer, page_size) != page_size) {
		perror("write");
		exit(1);
	    }
	}
    }
}

int
main(int argc, char **argv)
{
    int opt, i;

    page_size = sysconf(_SC_PAGESIZE);
    assert(page_size % 16 == 0);

    while((opt = getopt(argc, argv, "hd:i:o:")) != -1) {
	switch(opt) {
	    case 'd':
		dev_mem = optarg;
		break;

	    case 'i':
		input_seed = optarg;
		break;

	    case 'o':
		output_seed = optarg;
		break;

	    case 'h':
		usage(stdout);
		printf("\t-i seed\tverify prepared memory using seed\n"
		    "\t-o seed\tprepare memory using seed\n"
		    "\t-d dev\tuse dev as /dev/mem\n"
		    "\tarea: start+length or start-end\n"
		    "\t\tsuffix k for kilobytes, M for megabytes, p for pages\n"
		    "\n");
		return(0);

	    default:
		usage(stderr);
		return(1);
	}
    }
    if(optind == argc) {
	usage(stderr);
	return(1);
    }
    n_areas = argc - optind;
    areas = malloc(sizeof(memory_area_t) * n_areas);
    if(areas == NULL) {
	perror("malloc");
	return(1);
    }
    for(i = 0; i < n_areas; i++) {
	const char *a, *t;
	long l1, l2;
	char c;

	a = t = argv[optind + i];
	l1 = number(&t);
	if(t == NULL || l1 < 0) {
	    fprintf(stderr, "Invalid lower bound: '%s'\n", a);
	    return(1);
	}
	c = *(t++);
	if(c != '+' && c != '-') {
	    fprintf(stderr, "Invalid interval or lower unit: '%s'\n", a);
	    return(1);
	}
	l2 = number(&t);
	if(t == NULL || l2 < 0) {
	    fprintf(stderr, "Invalid upper bound: '%s'\n", a);
	    return(1);
	}
	if(*t != 0) {
	    fprintf(stderr, "Invalid upper unit: '%s'\n", a);
	    return(1);
	}
	if(c == '-') {
	    l2 -= l1;
	    if(l2 < 0) {
		fprintf(stderr, "Invalid interval: '%s'\n'", a);
		return(1);
	    }
	}
	if(l1 % page_size) {
	    fprintf(stderr, "Lower bound not on page boundary: '%s'\n'", a);
	    return(1);
	}
	if(l2 % page_size) {
	    fprintf(stderr, "Upper bound not on page boundary: '%s'\n'", a);
	    return(1);
	}
	areas[i].offset = l1;
	areas[i].n_pages = l2 / page_size;
    }

    if(input_seed == NULL && output_seed == NULL) {
	fprintf(stderr, "Nothing to do. Give at least one of -i or -o.\n");
	return(1);
    }
    if(dev_mem == NULL) {
	fprintf(stderr, "No device specified. Use the -d option.\n");
	return(1);
    }

    device = open(dev_mem, output_seed != NULL ? O_RDWR : O_RDONLY);
    if(device < 0) {
	perror(dev_mem);
	return(1);
    }

    buffer = malloc(page_size * 2);
    if(buffer == NULL) {
	perror("malloc");
	return(1);
    }
    if(input_seed != NULL)
	check_areas(input_seed, strlen(input_seed));
    if(output_seed != NULL)
	write_areas(output_seed, strlen(output_seed));

    return(0);
}
