#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "types.h"
#include "bitmap.h"
#include "winbmp.h"

static uint8 get_file_type (char *filename)
{
    //unsigned char *extend, *temp;
    char *extend, *temp;
    extend = rindex((char *)filename, '.');
    if (extend ==NULL)
        return IMAGE_TYPE_UNSUPPORT;
    extend++;
	if(strlen(extend) < 3 || strlen(extend) >4)
        return IMAGE_TYPE_UNSUPPORT;
    
	temp = extend;
	
	if(strcasecmp(temp, "bmp") == 0)
       	return IMAGE_TYPE_WINBMP;
    
	return IMAGE_TYPE_UNSUPPORT;
}


PBITMAP load_bitmap(char *filename)
{
	PBITMAP pBitmap = NULL;

    if(!filename)
    	return NULL;

    //printf("\nload bitmap %s\n", filename);
	uint8 type = get_file_type(filename);
//	printf("Image File type %d\n", type);
	switch(type)
	{
		case IMAGE_TYPE_WINBMP:
			pBitmap = load_winbmp(filename);
			break;
		default:
			fprintf(stderr, "Image File type %d isn't  support\n", type);
			break;
	}
    return pBitmap;
}


void print_bitmap_info(PBITMAP pBitmap) 
{ 
    if (pBitmap)
    {
    	printf("\nBITMAP info:\n");
    	printf("bmType %ld\n", pBitmap->bmType);
    	printf("bmWidth %ld\n", pBitmap->bmWidth);
    	printf("bmHeight %ld\n", pBitmap->bmHeight);
    	printf("bmWidthBytes %ld\n", pBitmap->bmWidthBytes);
    	printf("bmPlanes %d\n", pBitmap->bmPlanes);
    	printf("bmBitsPixel %d\n", pBitmap->bmBitsPixel);
    	printf("bmBits 0x%x\n", (uint32)pBitmap->bmBits);
    } 
}

int release_bitmap(PBITMAP pBitmap) 
{
    if(pBitmap)
    {	
    	if(pBitmap->bmPalettes)
    	    free(pBitmap->bmPalettes); 
    	if(pBitmap->bmBits)
    	    free(pBitmap->bmBits); 
    	free(pBitmap);
    }
    return 0;
}
	
int show_bitmap(PBITMAP pBitmap, int x, int y) 
{ 
    if (!pBitmap) 
		return -1; 
	switch(pBitmap->bmType)
	{
		case IMAGE_TYPE_WINBMP:
			show_winbmp(pBitmap, x, y); 
			break;
		default:
			fprintf(stderr, "Image File type %ld isn't  support\n", pBitmap->bmType );
			break;
	}
	return 0; 
} 
