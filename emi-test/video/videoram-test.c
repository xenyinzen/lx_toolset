/* 
 * memory_test.c
 * we allocate a big block of video memory (as large as total video ram) space to check  
 * 
 * tanggang -- 080121
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define BLOCK_SIZE	0x100000			// 1M
#define MEMORY_SIZE	0x400000			// 4M  SM712 MEM RAM capacity
#define START_ADDR_OFF	0x0			// 0


#define TST_PATTERN1	0xA53C5AC3

	
int main( int argc, char *argv[] )
{
	int fd, i, tmp;
	struct stat sb;
	char *region, *src;
	int retval = 0;	
	
	if ((fd = open("/dev/fb0", O_RDWR)) < 0) {
		perror("open");
		return -1;
	}
	
	for (i = 0; i < (MEMORY_SIZE - START_ADDR_OFF)/BLOCK_SIZE; i += 1) {
		region = mmap( 	NULL,
				BLOCK_SIZE, 
				PROT_READ|PROT_WRITE, 
				MAP_SHARED, 
				fd, 
				(unsigned int)(START_ADDR_OFF + i*BLOCK_SIZE) );
	printf("%d\n",i);
	if( region == (void *)-1 ){
			perror("mmap");
			return -1;
		}
		
/*
		if( mlock((void *)region, BLOCK_SIZE) ) {
			perror("mlock");
			return -1;
		}
*/
		
   		for( i = 0; i < BLOCK_SIZE; i += 4) {
			*(unsigned int *)(region + i) = TST_PATTERN1;
			tmp = *(unsigned int *)(region + i);
			
			if( tmp != TST_PATTERN1 ){
				return -1;
			}
		}

		munmap( region, BLOCK_SIZE );

	}
	/* TODO: */
	close(fd);
	return 0;
}

