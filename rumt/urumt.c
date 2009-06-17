/***************************************************************************
 * Running Unix Memory Test
 * (C) Nicolas George - 2003
 * urumt: full userland version
 ***************************************************************************/

#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <paths.h>

#include "rumt.h"

#define PROGRAMNAME "urumt"

#define LOGFACILITY LOG_LOCAL0
#define INT_SIZE 4

const char *dev_mem_path = _PATH_MEM;
const char *stats_path = "urumt_stats";
unsigned long n_pages = 0;
unsigned delay = 900, delay_mod = 1;
unsigned long max_bugs_panic = 16;

/*#define FORCE_BUGS 10*/

long page_size;
volatile unsigned char *arena;
unsigned char *mem_buf;
int dev_mem;
int stats;
long *page_map;
unsigned long pid;
unsigned long magic = 0xDEADBEEF;
void (*message)(int, const char *, ...);

/***************************************************************************
 * Error logging
 ***************************************************************************/

static void
stdout_messagev(int priority, const char *fmt, va_list va)
{
    struct timeval tv;
    struct tm *tm;
    FILE *out;
    size_t l;

    out = priority == LOG_ERR ? stderr : stdout;
    gettimeofday(&tv, NULL);
    tm = gmtime(&tv.tv_sec);
    if(tm == NULL)
	abort();
    fprintf(out, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d.%3.3d %s: ",
	tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	tm->tm_hour, tm->tm_min, tm->tm_sec,
	(int)(tv.tv_usec / 1000), PROGRAMNAME);
    vfprintf(out, fmt, va);
    l = strlen(fmt);
    if(l > 0 && fmt[l - 1] == ':') {
	char *e;

	e = strerror(errno);
	if(e == NULL)
	    e = "unknown error";
	fprintf(out, " %s\n", e);
    } else {
	fputc('\n', out);
    }
    fflush(out);
}

static void
stdout_message(int priority, const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    stdout_messagev(priority, fmt, va);
    va_end(va);
}

static void
fatal_error(const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    stdout_messagev(LOG_ERR, fmt, va);
    va_end(va);
    exit(1);
}

/***************************************************************************
 * Program testing
 ***************************************************************************/

#if defined(FORCE_BUGS) && FORCE_BUGS > 0

#include <signal.h>

static void
force_bug(int sig)
{
    (void)sig;
    unsigned long page, byte;
    int bit;

    page = rand() % n_pages;
    byte = rand() % page_size;
    bit = rand() % 8;
    arena[page * page_size + byte] ^= 1 << bit;
    write(2, "Randomized.\n", 12);
    alarm(FORCE_BUGS);
}

static void
prepare_force_bug(void)
{
    struct sigaction s;

    s.sa_handler = force_bug;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
    if(sigaction(SIGALRM, &s, NULL))
	fatal_error("sigaction:");
    alarm(FORCE_BUGS);
}

#endif /* FORCE_BUGS */

/***************************************************************************
 * Random content
 ***************************************************************************/

static void
prepare_area(unsigned long page, unsigned long pass, size_t size,
    volatile unsigned char *dst)
{
    unsigned long seed[4];

    md5stream_init();
    seed[0] = page;
    seed[1] = pass;
    seed[2] = pid;
    seed[3] = magic;
    md5stream_seed((void *)seed, sizeof(seed));
    md5stream_fill((void *)dst, size);
}

/***************************************************************************
 * Memory manipulation
 ***************************************************************************/

static int
get_page(unsigned long page, ssize_t len)
{
    if(len == 0)
	len = page_size;
    if(pread(dev_mem, mem_buf, len, page * page_size) != len) {
	if(errno == EFAULT)
	    return(-1);
	fatal_error("pread(/dev/mem):");
    }
    return(0);
}

void
look_for_pages(void)
{
    unsigned long i, e, f = 0;
    volatile unsigned long *p, *mb;

    for(i = 0; i < n_pages; i++) {
	p = (volatile long *)(arena + i * page_size);
	p[0] = i;
	p[1] = magic;
	p[2] = pid;
	p[3] = n_pages - i;
	page_map[i] = -1;
    }
    i = 0;
    mb = (unsigned long *)mem_buf;
    for(i = 0; get_page(i, 4 * sizeof(long)) == 0; i++) {
	e = mb[0];
	if(e >= n_pages || mb[1] != magic || mb[2] != pid ||
	    mb[3] != n_pages - e)
	    continue;
	if(page_map[e] >= 0)
	    fatal_error("Pages 0x%lx and 0x%lx look the same. Aborting.\n",
		page_map[e], i);
	page_map[e] = i;
	f++;
    }
    message(LOG_INFO, "Available memory is %ld pages.", i);
    message(LOG_INFO, "Found %ld/%ld pages (%ld%%).",
	f, n_pages, 100 * f / n_pages);
}

/***************************************************************************
 * Main loop
 ***************************************************************************/

static void
record_incr(unsigned long offset, unsigned long inc)
{
    unsigned char buf[INT_SIZE];
    ssize_t r;
    unsigned long v = 0, v2;
    int i;

    offset *= INT_SIZE;
    while((r = pread(stats, buf, INT_SIZE, offset)) < 0) {
	if(errno == EINTR)
	    continue;
	fatal_error("pread(stats):");
    }
    if(r == 0)
	memset(buf, 0, INT_SIZE);
    else if(r != INT_SIZE)
	fatal_error("pread(stats): read %d", (int)r);
    for(i = 0; i < INT_SIZE; i++)
	v = v * 256 + buf[i];
    v2 = v + inc;
    if(v2 < v) {
	memset(buf, 255, INT_SIZE);
    } else {
	for(i = INT_SIZE - 1; i >= 0; i--) {
	    buf[i] = v2 & 255;
	    v2 /= 256;
	}
    }
    while((r = pwrite(stats, buf, INT_SIZE, offset)) != INT_SIZE) {
	if(errno == EINTR)
	    continue;
	fatal_error("pwrite(stats):");
    }
}

static void
record_bad_bits(unsigned long page, unsigned long n)
{
    record_incr(page * 2 + 1, n);
    if(fsync(stats))
	fatal_error("fsync:");
}

static void
record_checked(unsigned long page)
{
    record_incr(page * 2, 1);
}

void
do_tests(void)
{
    unsigned long ipage = 0, ipass = 0;
    unsigned long opage = 0, opass = 0;
    unsigned long retard;
    unsigned delay_count = delay_mod;
    unsigned pass = 0;
    unsigned long byte, n_errors;
    volatile unsigned char *page;
    int bit, diff;

    retard = n_pages - 1;
    while(1) {
	if(--delay_count == 0) {
	    usleep(delay);
	    delay_count = delay_mod;
	}
	page = arena + ipage * page_size;
	prepare_area(ipage, ipass, page_size, page);
	ipage = (ipage + 1) % n_pages;
	ipass++;
	if(ipage == 0) {
	    message(LOG_INFO, "Pass %d.", ++pass);
	}
	if(retard) {
	    retard--;
	    continue;
	}
	prepare_area(opage, opass, page_size, mem_buf);
	page = arena + opage * page_size;
	if(memcmp(mem_buf, (void *)page, page_size)) {
	    n_errors = 0;
	    for(byte = 0; byte < (unsigned long)page_size; byte++) {
		if((diff = (mem_buf[byte] ^ page[byte])) == 0)
		    continue;
		for(bit = 0; bit < 8; bit++)
		    if((diff & (1 << bit)) != 0) {
			message(LOG_CRIT, "bad bit: %8.8lx:%3.3lx.%d %c",
			    page_map[opage], byte, bit,
			    (mem_buf[byte] & (1 << bit)) ? '+' : '-');
			n_errors++;
		    }
	    }
	    if(n_errors > max_bugs_panic)
		fatal_error("too many bad bits, I am in bad memory? Aborting.");
	    if(page_map[opage] < 0) {
		message(LOG_CRIT, "bad bits in unknown page.");
	    } else {
		record_bad_bits(page_map[opage], n_errors);
	    }
	}
	if(page_map[opage] >= 0)
	    record_checked(page_map[opage]);
	opage = (opage + 1) % n_pages;
	opass++;
    }
}

static void
print_stats(void)
{
    unsigned char buf[2][INT_SIZE];
    unsigned long page, pass, bad;
    int i;

    if((stats = open(stats_path, O_RDONLY)) < 0)
	fatal_error("open(%s):", stats_path);
    for(page = 0; read(stats, buf, 2 * INT_SIZE) == 2 * INT_SIZE; page++) {
	pass = bad = 0;
	for(i = 0; i < INT_SIZE; i++) {
	    pass = pass * 256 + buf[0][i];
	    bad = bad * 256 + buf[1][i];
	}
	printf("%12lx\t%12lu\t%12lu\n", page, pass, bad);
    }
}

static void
usage(FILE *out)
{
    fprintf(out, "Usage:\t" PROGRAMNAME " [options] -p pages\n"
	"\t" PROGRAMNAME " [options] -P\n"
	"Options:\n"
	"\t-s statfile\n"
	"\t-m memdevice\n"
	"\t-d delay\n"
	"\t-D delay_mod\n"
	"\t-b badbits\n"
	"\t-S\n");
}

int
main(int argc, char **argv)
{
    int o;

    message = stdout_message;
    while((o = getopt(argc, argv, "p:Ps:m:d:D:b:Sh")) != -1) {
	switch(o) {
	    case 'p':
		n_pages = strtol(optarg, NULL, 0);
		break;
	    case 'P':
		print_stats();
		return(0);
	    case 's':
		stats_path = optarg;
		break;
	    case 'm':
		dev_mem_path = optarg;
		break;
	    case 'd':
		delay = strtol(optarg, NULL, 0);
		break;
	    case 'D':
		delay_mod = strtol(optarg, NULL, 0);
		if(delay_mod < 1)
		    delay_mod = 1;
		break;
	    case 'b':
		max_bugs_panic = strtol(optarg, NULL, 0);
		if(max_bugs_panic < 1)
		    max_bugs_panic = 1;
		break;
	    case 'S':
		openlog(PROGRAMNAME, LOG_PID, LOGFACILITY);
		message = syslog;
		break;
	    case 'h':
		usage(stdout);
		return(0);
	    default:
		usage(stderr);
		return(1);
	}
    }
    if(argc > optind || n_pages < 2) {
	usage(stderr);
	return(1);
    }
    page_size = sysconf(_SC_PAGE_SIZE);
    pid = getpid();
    if(page_size < 0) {
	perror(PROGRAMNAME ": getconf(PAGE_SIZE)");
	exit(1);
    }
    message(LOG_INFO, "Page size is %ld.", page_size);
    if((mem_buf = malloc(page_size + sizeof(long long))) == NULL)
	fatal_error("malloc:");
    if((long)mem_buf % page_size == 0)
	/* we do not want mem_buf page-aligned */
	mem_buf += sizeof(long long);
    if((page_map = malloc(n_pages * sizeof(long))) == NULL)
	fatal_error("malloc:");
    if((arena = mmap(NULL, page_size * n_pages, PROT_READ | PROT_WRITE,
	MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
	fatal_error("mmap:");
    if(mlock((void *)arena, page_size * n_pages)) /* discard volatile */
	fatal_error("mlock:");
    if((dev_mem = open(dev_mem_path, O_RDONLY)) < 0)
	fatal_error("open(%s):", dev_mem_path);
    if((stats = open(stats_path, O_RDWR | O_CREAT, 0666)) < 0)
	fatal_error("open(%s):", stats_path);
    look_for_pages();
#if defined(FORCE_BUGS) && FORCE_BUGS > 0
    prepare_force_bug();
#endif
    do_tests();
    return(0);
}
