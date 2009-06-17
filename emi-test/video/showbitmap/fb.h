#ifndef _FB_H
#define _FB_H
#include <linux/fb.h>

typedef struct _fb_screen
{
	uint32   type; 
	uint32   visual; 
	uint32   width; 
	uint32   height; 
	uint32   pitch; 
	uint32   bits_per_pixel; 
	uint8 *  start; 
} SCREEN, *PSCREEN; 

int fb_init(); 
int fb_uninit();
void fb_screen_info(); 
PSCREEN fb_get_screen();
int fb_set_cmap(struct fb_cmap * cmap);
void restore_cmap();

#endif
