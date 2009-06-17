#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "types.h"
#include "fb.h"

#define RGB(r,g,b)	(unsigned short) ( (r>>3) | ( (g>>2)<<5 ) | (b << 11) ) 
#define BLOCKWIDTH 80
#define BLOCKHEIGHT 48

unsigned short color[4] = { RGB(0xff, 0, 0), RGB(0, 0xff, 0), RGB(0, 0, 0xff), RGB(0xff, 0xff, 0xff)};


				
static void usage(char *program)
{
	fprintf(stderr, "FrameBuffer test demo.\n");
	fprintf(stderr, "Usage: %s [options] -t number\n", program);
}

int main( int argc, char *argv[] ) 
{ 
    int type = 0;
    int c;
		
    while ((c = getopt(argc, argv,"t:h")) != -1)
    {
        switch (c)
        {
            case 't':
                type = strtol(optarg, NULL, 10);
                break;
				
            case 'h':
            default:
                usage(argv[0]);
                return 1;
                break;
        }
    }

    printf("tyep = %d\n", type);
    if(type == 0)
        return 1;
				

    if(fb_init() != 0)
    {
    	fprintf(stderr, "init_screen failed\n");
    	return -1;
    }

    PSCREEN screen = fb_get_screen();
    
    int bytespix = screen->bits_per_pixel / 8;
    int w = screen->width;
    int h = screen->height;
    memset(screen->start, 0, screen->pitch * h);
    printf("bpp = %d, (%dx%x)\n", bytespix, w, h);

    unsigned char * dest;
    unsigned short * idest;
    int n, i, j;
    int x, y;
    switch (type)
    {
        case 1:
		
            for(n = 0; n<4; n++)
            {
                for(i = 0; i < (h/4); i++)
                {
                	dest = (unsigned char *)screen->start + (n * (h/4) + i) * screen->pitch;
                	idest = (unsigned short *)dest; 
                	for(j = 0; j < w; j++)
                	{
                		*(idest+j) = (unsigned short)color[n];	
             		}
              	}
            }
            break;
        case 2:
        	  x = BLOCKWIDTH;
        	  y = BLOCKHEIGHT;
        	  for(i = 0; i < BLOCKHEIGHT; i++)
        	  {
        	      dest = screen->start + (y + i) * screen->pitch + x * bytespix;
                idest = (UINT *)dest; 
                for(j = 0; j < BLOCKWIDTH; j++)
                {
                    *(idest+j) = RGB(0xff, 0xff, 0xff);	
             		}
        	  }
        	  
         	  x = w - BLOCKWIDTH * 2;
        	  y = BLOCKHEIGHT;
        	  for(i = 0; i < BLOCKHEIGHT; i++)
        	  {
        	      dest = screen->start + (y + i) * screen->pitch + x * bytespix;
                idest = (UINT *)dest; 
                for(j = 0; j < BLOCKWIDTH; j++)
                {
                    *(idest+j) = RGB(0xff, 0xff, 0xff);	
             		}
        	  }
        	  
        	  x = BLOCKWIDTH;
        	  y = h - BLOCKHEIGHT * 2;
        	  for(i = 0; i < BLOCKHEIGHT; i++)
        	  {
        	      dest = screen->start + (y + i) * screen->pitch + x * bytespix;
                idest = (UINT *)dest; 
                for(j = 0; j < BLOCKWIDTH; j++)
                {
                    *(idest+j) = RGB(0xff, 0xff, 0xff);	
             		}
        	  }
        	  
        	  x = w - BLOCKWIDTH * 2;
        	  y = h - BLOCKHEIGHT * 2;
        	  for(i = 0; i < BLOCKHEIGHT; i++)
        	  {
        	      dest = screen->start + (y + i) * screen->pitch + x * bytespix;
                idest = (UINT *)dest; 
                for(j = 0; j < BLOCKWIDTH; j++)
                {
                    *(idest+j) = RGB(0xff, 0xff, 0xff);	
             		}
        	  }
        	    
        	  x = (w - BLOCKWIDTH) /2;
        	  y = (h - BLOCKHEIGHT) / 2;
        	  for(i = 0; i < BLOCKHEIGHT; i++)
        	  {
        	      dest = screen->start + (y + i) * screen->pitch + x * bytespix;
                idest = (UINT *)dest; 
                for(j = 0; j < BLOCKWIDTH; j++)
                {
                    *(idest+j) = RGB(0xff, 0xff, 0xff);	
             		}
        	  }
            break;
    }

    fb_uninit();
    return 0; 
} 
