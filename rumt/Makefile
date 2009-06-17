###########################################################################
# Running Unix Memory Test
# (C) Nicolas George - 2003
# Makefile: guess what
###########################################################################

CC=gcc
CFLAGS=-Wall -W -std=c99 -D_XOPEN_SOURCE=600 -g -O3
LDFLAGS=
LIBS=
RM=rm

#CC_K=$(CC)
CC_K=gcc-2.95
CFLAGS_K=$(CFLAGS) -D__KERNEL__ -DMODULE -I/usr/src/linux/include

TRYMEM_OBJECTS=rumt_trymem.o md5stream.o
URUMT_OBJECTS=urumt.o md5stream.o

.c.o:
	$(CC) -c $(CFLAGS) $<

all: rumt_trymem bgrep urumt

rumt_trymem: $(TRYMEM_OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(TRYMEM_OBJECTS) $(LIBS)

urumt: $(URUMT_OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(URUMT_OBJECTS) $(LIBS)

bgrep: bgrep.c
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) bgrep.c $(LIBS)

rumt_trymem.o: rumt.h
md5stream.o: rumt.h

highmem.o: highmem.c
	$(CC_K) $(CFLAGS_K) -c highmem.c

clean:
	$(RM) -f rumt_trymem urumt bgrep $(TRYMEM_OBJECTS) $(URUMT_OBJECTS) \
	  highmem.o
