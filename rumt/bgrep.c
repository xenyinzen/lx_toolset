/***************************************************************************
 * Running Unix Memory Test
 * (C) Nicolas George - 2003
 * bgrep: binary grep
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int verbose = 0;
unsigned char *look_for;
unsigned n_look_for = 0;
unsigned look_for_allocated;
unsigned *look_for_link;

static void
usage(FILE *f)
{
    fprintf(f, "Usage: bgrep [-v] bytes\n");
}

int
push_hex_digit(unsigned char x, int *v)
{
    x |= 32;
    if(x >= '0' && x <= '9')
	*v = *v * 16 + (x - '0');
    else if(x >= 'a' && x <= 'f')
	*v = *v * 16 + (x - 'a' + 10);
    else
	return(1);
    return(0);
}

static void
make_kmp_links(void)
{
    int i, j;

    look_for_link = calloc(n_look_for, sizeof(unsigned));
    if(look_for_link == NULL) {
	perror("malloc");
	exit(1);
    }
    for(i = 0; i < n_look_for; i++)
	look_for_link[i] = n_look_for - 1;
    /* Assertion: for all i
     * look_for[i+1 .. ] == look_for[link[i]+1 .. ]
     */
    for(i = 0; i < n_look_for; i++) {
	for(j = i + 2; j < n_look_for; j++) {
	    if(memcmp(look_for + i + 1, look_for + j, n_look_for - j) == 0) {
		look_for_link[i] = j - 1;
		break;
	    }
	}
    }
}

int
main(int argc, char **argv)
{
    {
	int o;

	while((o = getopt(argc, argv, "vh")) != -1) {
	    switch(o) {
		case 'v':
		    verbose++;
		    break;
		default:
		    usage(o == 'h' ? stdout : stderr);
		    exit(o == 'h' ? 0 : 1);
	    }
	}
    }
    look_for_allocated = (argc - optind) * 2; /* guess */
    if(look_for_allocated == 0) {
	usage(stderr);
	return(1);
    }
    look_for = malloc(look_for_allocated);
    if(look_for == NULL) {
	perror("malloc");
	return(1);
    }
    {
	int i, a = 1;
	unsigned char *p, *q;

	for(i = optind; i < argc; i++) {
	    for(p = argv[i]; *p != 0; p++) {
		if(push_hex_digit(*p, &a)) {
		    fprintf(stderr, "Bad hex digit (%d+%d)\n", i,
			p - (unsigned char *)argv[i]);
		    return(1);
		}
		if(a & 0x100) {
		    if(n_look_for == look_for_allocated) {
			look_for_allocated *= 2;
			look_for = realloc(look_for, look_for_allocated);
			if(look_for == NULL) {
			    perror("malloc");
			    return(1);
			}
		    }
		    look_for[n_look_for++] = a & 0xFF;
		    a = 1;
		}
	    }
	    if(a != 1) {
		fprintf(stderr, "Truncated byte (%d)\n", i);
		return(1);
	    }
	}
	look_for = realloc(look_for, n_look_for);
	/* This is a hack: since this tool is in particular intended to grep
	 * the memory, we do not want to find our own copy of the bytes. So
	 * we exchange them. */
	for(p = look_for, q = look_for + n_look_for - 1; p < q; p++, q--) {
	    unsigned char t;

	    t = *p;
	    *p = *q;
	    *q = t;
	}
    }
    make_kmp_links();
    {
	unsigned long long o = 0;
	unsigned p;
	unsigned char buf[4096];
	int l, i;

	p = n_look_for - 1;
	while(1) {
	    l = fread(buf, 1, sizeof(buf), stdin);
	    if(l <= 0)
		break;
	    for(i = 0; i < l; i++) {
		o++;
		while(p != n_look_for - 1 && buf[i] != look_for[p])
		    p = look_for_link[p];
		if(buf[i] == look_for[p]) {
		    if(p == 0) {
			printf("0x%12.12llx\n", o - n_look_for);
			p = look_for_link[p];
		    } else {
			p--;
		    }
		}
	    }
	    if(verbose) {
		if(verbose > 2)
		    fprintf(stderr, "%lluk\r", o >> 10);
		else if((o & 0xFFFFF) == 0)
		    fprintf(stderr, "%lluM\r", o >> 20);
	    }
	}
    }
    return(0);
}
