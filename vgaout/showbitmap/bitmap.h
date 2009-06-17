#ifndef _BITMAP_H
#define _BITMAP_H

#define IMAGE_TYPE_UNSUPPORT		0xff
#define IMAGE_TYPE_NONE		0x0
#define IMAGE_TYPE_WINBMP 	0x1
#define IMAGE_TYPE_JPEG 	0x2
#define IMAGE_TYPE_PNG 		0x3
#define IMAGE_TYPE_GIF	 	0x4

typedef struct tagBITMAP {
  LONG   bmType; 
  LONG   bmWidth; 
  LONG   bmHeight; 
  LONG   bmWidthBytes; 
  WORD   bmPlanes; 
  WORD   bmBitsPixel; 
  LONG	 bmPalNum; 
  LPLONG bmPalettes; 
  BYTE * bmBits; 
} BITMAP, *PBITMAP; 

int show_bitmap(PBITMAP pBitmap, int x, int y);
int release_bitmap(PBITMAP pBitmap);
void print_bitmap_info(PBITMAP pBitmap);
PBITMAP load_bitmap(char *filename);

#endif

