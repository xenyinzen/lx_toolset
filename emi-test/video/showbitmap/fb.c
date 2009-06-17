#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h> 
#include <sys/mman.h> 
#include <sys/ioctl.h> 
#include <arpa/inet.h> 
#include "types.h"
#include "fb.h"

static PSCREEN fb_screen = NULL;
static int console_fd = -1;
static struct fb_cmap saved_cmap;


//#define FB_DEBUG
#ifdef FB_DEBUG
static void print_vinfo(struct fb_var_screeninfo *vinfo)
{
    fprintf(stderr, "Printing vinfo:\n");
    fprintf(stderr, "txres: %d\n", vinfo->xres);
    fprintf(stderr, "tyres: %d\n", vinfo->yres);
    fprintf(stderr, "txres_virtual: %d\n", vinfo->xres_virtual);
    fprintf(stderr, "tyres_virtual: %d\n", vinfo->yres_virtual);
    fprintf(stderr, "txoffset: %d\n", vinfo->xoffset);
    fprintf(stderr, "tyoffset: %d\n", vinfo->yoffset);
    fprintf(stderr, "tbits_per_pixel: %d\n", vinfo->bits_per_pixel);
    fprintf(stderr, "tgrayscale: %d\n", vinfo->grayscale);
    fprintf(stderr, "tnonstd: %d\n", vinfo->nonstd);
    fprintf(stderr, "tactivate: %d\n", vinfo->activate);
    fprintf(stderr, "theight: %d\n", vinfo->height);
    fprintf(stderr, "twidth: %d\n", vinfo->width);
    fprintf(stderr, "taccel_flags: %d\n", vinfo->accel_flags);
    fprintf(stderr, "tpixclock: %d\n", vinfo->pixclock);
    fprintf(stderr, "tleft_margin: %d\n", vinfo->left_margin);
    fprintf(stderr, "tright_margin: %d\n", vinfo->right_margin);
    fprintf(stderr, "tupper_margin: %d\n", vinfo->upper_margin);
    fprintf(stderr, "tlower_margin: %d\n", vinfo->lower_margin);
    fprintf(stderr, "thsync_len: %d\n", vinfo->hsync_len);
    fprintf(stderr, "tvsync_len: %d\n", vinfo->vsync_len);
    fprintf(stderr, "tsync: %d\n", vinfo->sync);
    fprintf(stderr, "tvmode: %d\n", vinfo->vmode);
    fprintf(stderr, "tred: %d/%d\n", vinfo->red.length, vinfo->red.offset);
    fprintf(stderr, "tgreen: %d/%d\n", vinfo->green.length, vinfo->green.offset);
    fprintf(stderr, "tblue: %d/%d\n", vinfo->blue.length, vinfo->blue.offset);
    fprintf(stderr, "talpha: %d/%d\n", vinfo->transp.length, vinfo->transp.offset);
}
static void print_finfo(struct fb_fix_screeninfo *finfo)
{
    fprintf(stderr, "Printing finfo:\n");
    fprintf(stderr, "tsmem_start = %p\n", (char *)finfo->smem_start);
    fprintf(stderr, "tsmem_len = %d\n", finfo->smem_len);
    fprintf(stderr, "ttype = %d\n", finfo->type);
    fprintf(stderr, "ttype_aux = %d\n", finfo->type_aux);
    fprintf(stderr, "tvisual = %d\n", finfo->visual);
    fprintf(stderr, "txpanstep = %d\n", finfo->xpanstep);
    fprintf(stderr, "typanstep = %d\n", finfo->ypanstep);
    fprintf(stderr, "tywrapstep = %d\n", finfo->ywrapstep);
    fprintf(stderr, "tline_length = %d\n", finfo->line_length);
    fprintf(stderr, "tmmio_start = %p\n", (char *)finfo->mmio_start);
    fprintf(stderr, "tmmio_len = %d\n", finfo->mmio_len);
    fprintf(stderr, "taccel = %d\n", finfo->accel);
}

static void print_cmap(struct fb_cmap *cmap)
{
    int i;
    uint r, g, b, transp; 
    printf("colormap len %d\n", cmap->len);
    for(i=0; i<cmap->len; i++)
	{
		r=*(cmap->red+i);
		g=*(cmap->green+i);
		b=*(cmap->blue+i);
		transp=*(cmap->blue+i);
		printf("%3d   0x%x, 0x%x, 0x%x, 0x%x\n", i, r, g, b, transp);		
	}
}
#endif

void fb_screen_info() 
{ 
    if (fb_screen)
    {
    	printf("\nFrameBuffer Screen info:\n");
    	printf("type  %d\n", fb_screen->type);
    	printf("visual %d\n", fb_screen->visual);
    	printf("width  %d\n", fb_screen->width);
    	printf("height %d\n", fb_screen->height);
    	printf("pitch  %d\n", fb_screen->pitch);
    	printf("bits_per_pixel %d\n", fb_screen->bits_per_pixel);
    	printf("start 0x%x\n", (uint32)fb_screen->start);
    } 
	else
    	fprintf(stderr, "\nNo FrameBuffer Screen\n");
}

static int fb_get_cmap(struct fb_cmap * cmap)
{
	if(!cmap)
		return -1;
    return ioctl(console_fd, FBIOGETCMAP, cmap);
}

int fb_set_cmap(struct fb_cmap * cmap)
{
	if(!cmap)
		return -1;
    return ioctl(console_fd, FBIOPUTCMAP, cmap);
}

void restore_cmap()
{
	fb_set_cmap(&saved_cmap);
}

int fb_init() 
{ 
    int fd = 0; 
    struct fb_var_screeninfo vinfo; 
    struct fb_fix_screeninfo finfo; 
    uint32 screensize = 0; 

    // Open the file for reading and writing 
    fd = open("/dev/fb0", O_RDWR); 
    if (fd < 0) 
    { 
        fprintf(stderr, "Error: cannot open framebuffer device.\n"); 
        close(fd);
        return -1; 
    } 

    // Get fixed screen information 
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo)) 
    { 
        fprintf(stderr, "Error reading fixed information.\n"); 
        close(fd);
        return -1; 
    } 

    // Get variable screen information 
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) 
    { 
        printf("Error reading variable information.\n"); 
        close(fd);
        return -1; 
    } 

#ifdef FB_DEBUG
    print_finfo(&finfo);
    print_vinfo(&vinfo);
    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel ); 
#endif

    console_fd = fd;
    //my_set_palette(&finfo, &vinfo);
 
    // Figure out the size of the screen in bytes 
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8; 

    // Map the device to memory 
    unsigned char * fbp = (unsigned char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                       fd, 0); 
    if ((int)fbp == -1) 
    { 
        printf("Error: failed to map framebuffer device to memory.\n"); 
        close(fd);
        return -1; 
    } 
	
	memset(fbp, 0, screensize);

	int cmaplen = 0;
	uint16  *color = NULL;
	
    if(finfo.visual == FB_VISUAL_PSEUDOCOLOR )
	{
        cmaplen = 1 << vinfo.bits_per_pixel;
        color = (uint16*)malloc(3 * cmaplen * sizeof(uint16));
        if( !color)
    	{
    		close(fd); 
 			return -1;
    	}
		memset(&saved_cmap, 0, sizeof(saved_cmap));
		saved_cmap.len = cmaplen;
		saved_cmap.red = color;
		saved_cmap.green = color + cmaplen;
		saved_cmap.blue = color + cmaplen*2;
			
		fb_get_cmap(&saved_cmap);
#ifdef FB_DEBUG
		print_cmap(&saved_cmap);
#endif
    }
    
    fb_screen = (PSCREEN)malloc(sizeof(SCREEN));
    if(!fb_screen)
    {
    	munmap(fbp, screensize); 
    	close(fd); 
    	return -1;
    }
    
    memset(fb_screen, 0, sizeof(SCREEN));
    fb_screen->visual = finfo.visual; 
    fb_screen->width = vinfo.xres; 
    fb_screen->height  = vinfo.yres; 
    fb_screen->bits_per_pixel = vinfo.bits_per_pixel; 
    fb_screen->pitch = vinfo.xres * vinfo.bits_per_pixel / 8; 
    fb_screen->start = fbp;

    //fb_screen_info();
    return 0; 
} 
 
int fb_uninit()
{ 
	if(saved_cmap.red)
		free(saved_cmap.red);
    if(fb_screen)
    {	
    	if(fb_screen->start)
     	    munmap(fb_screen->start, fb_screen->pitch * fb_screen->height); 
    	free(fb_screen);
    }
	if(console_fd>0)
	{
    	close(console_fd);
		console_fd = -1; 
	}
    return 0;
}

PSCREEN fb_get_screen()
{
	return fb_screen;
}
