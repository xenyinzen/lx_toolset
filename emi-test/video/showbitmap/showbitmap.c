#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include "types.h"
#include "fb.h"
#include "bitmap.h"


int main( int argc, char *argv[] ) 
{ 
    if(argc < 2)
    {
		printf("usage: ./showbitmap filename\n");
	}

    if(fb_init() != 0)
	{
    	fprintf(stderr, "init_screen failed\n");
    	return -1;
	}
   	
    PBITMAP pBmp = load_bitmap(argv[1]);
    int x=0, y=0;
    PSCREEN g_pScreen = fb_get_screen();
    if(pBmp)
    {
	x = (g_pScreen->width - pBmp->bmWidth)/2;	
	y = (g_pScreen->height - pBmp->bmHeight)/2;	
	if(x>=0 && y>=0)
    		show_bitmap(pBmp, x, y);
	else
		fprintf(stderr, "Error: screen (%dx%d) < bitmap (%dx%d)\n", 
			g_pScreen->width, g_pScreen->height,	
			pBmp->bmWidth, pBmp->bmHeight);	
    	release_bitmap(pBmp);
    }
    fb_uninit();

    return 0; 
} 
