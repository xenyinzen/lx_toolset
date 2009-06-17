#ifndef _WINBMP_H
#define _WINBMP_H

typedef struct tagBITMAPFILEHEADER { 
#if 1
	UINT bfType;
	DWORD bfSize;
	UINT bfReserved1;
	UINT bfReserved2;
	DWORD bfOffBits;
#else
	char  bfType[2];
	DWORD bfSize;
	DWORD bfReserved;
	DWORD bfOffBits;
#endif
} BITMAPFILEHEADER;

typedef struct tagRGBQUAD {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER { 
	DWORD biSize;
	long biWidth;
	long biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	long biXPelsPerMeter;
	long biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
} BITMAPINFO;

PBITMAP load_winbmp(char *filename); 
int show_winbmp(PBITMAP pBitmap, int x, int y);

#endif
