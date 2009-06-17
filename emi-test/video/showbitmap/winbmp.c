#include <unistd.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "types.h"
#include "bitmap.h"
#include "winbmp.h"
#include "fb.h"

//#define _DEBUG

PBITMAP load_winbmp(char *filename)
{
    FILE * fp;
    if(!filename)
    	return NULL;
    //printf("\nload win bmp %s\n", filename);
    if((fp = fopen(filename,"rb")) == NULL)
    {
		fprintf(stderr, "open bmp file %s error!", filename);
		return NULL;
    }
    
    BITMAPFILEHEADER bmfHeader;
#ifdef _DEBUG
    printf("\nBITMAPFILEHEADER %d\n", sizeof(bmfHeader));
#endif

    fread(&bmfHeader.bfType, sizeof(bmfHeader.bfType), 1, fp);
#ifdef _DEBUG
    printf("bfType 0x%x\n", bmfHeader.bfType);
#endif
    if(bmfHeader.bfType != 0x4d42)
    {
    	printf("This is not a bmp file\n");
		fclose(fp);
    	return NULL;
    }
    fread(&bmfHeader.bfSize, sizeof(bmfHeader)-4, 1, fp);
#ifdef _DEBUG
    printf("bfSize %ld\n", bmfHeader.bfSize);
    printf("bfOffBits 0x%lx\n", bmfHeader.bfOffBits);
#endif

    BITMAPINFOHEADER bmiHeader;
    fread(&bmiHeader, sizeof(bmiHeader), 1, fp);
#ifdef _DEBUG
    printf("\nBITMAPINFOHEADER %d\n", sizeof(bmiHeader));
    printf("biSize 0x%lx\n", bmiHeader.biSize);
    printf("biWidth %ld\n", bmiHeader.biWidth);
    printf("biHeight %ld\n", bmiHeader.biHeight);
    printf("biPlanes %d\n", bmiHeader.biPlanes);
    printf("biBitCount %d\n", bmiHeader.biBitCount);
    printf("biCompression %ld\n", bmiHeader.biCompression);
    printf("biXPelsPerMeter %ld\n", bmiHeader.biXPelsPerMeter);
    printf("biYPelsPerMeter %ld\n", bmiHeader.biYPelsPerMeter);
    printf("biClrUsed %ld\n", bmiHeader.biClrUsed);   
    printf("biClrImportant %ld\n", bmiHeader.biClrImportant);  
#endif
	
    PBITMAP pBmp = (PBITMAP)malloc(sizeof(BITMAP));
	if(pBmp == NULL)
    {
		fclose(fp);
    	return NULL;
    }
	memset(pBmp, 0, sizeof(BITMAP));
	pBmp->bmType = IMAGE_TYPE_WINBMP; 
	pBmp->bmWidth = bmiHeader.biWidth; 
	pBmp->bmHeight = bmiHeader.biHeight; 
  	pBmp->bmWidthBytes = ((bmiHeader.biWidth+3)/4) * 4 * bmiHeader.biBitCount / 8; 
  	pBmp->bmPlanes = bmiHeader.biPlanes; 
  	pBmp->bmBitsPixel = bmiHeader.biBitCount; 

   	RGBQUAD * palette; 
	int nums=(bmfHeader.bfOffBits-54)/4;
	
	if(nums >0)
	{
		palette = (RGBQUAD*)malloc(sizeof(RGBQUAD)*nums);
		fread(palette, sizeof(RGBQUAD)*nums, 1, fp);
		pBmp->bmPalNum = nums;
		pBmp->bmPalettes = (LPLONG)palette;
#if 0			
	int i;
    LONG tmp;	
	for(i=0; i<nums; i++)
	{
		tmp = (LONG)palette[i].rgbRed + (LONG)palette[i].rgbGreen * 0x100 + (LONG)palette[i].rgbBlue * 0x10000;
		//printf("RGB 0x%x, 0x%x, 0x%x === 0x%x === 0x%x\n", 
		palette[i].rgbRed, palette[i].rgbGreen, palette[i].rgbBlue,
					*((LPLONG*)&palette[i]), tmp);
	}	
#endif
	}

	fseek(fp, bmfHeader.bfOffBits, SEEK_SET);

	unsigned char * bits = (unsigned char *)malloc(bmfHeader.bfSize - bmfHeader.bfOffBits); 
	if(!bits)
	{
		free(pBmp);
		fclose(fp);
    	return NULL;
	}
	pBmp->bmBits = bits;	
	#if 1
	//right-bottom data is first line
	fread(pBmp->bmBits, bmfHeader.bfSize - bmfHeader.bfOffBits, 1, fp);
	#else
	int l;
	for(l=0; l<pBmp->bmHeight; l++)
	{
		fread(bits+pBmp->bmWidthBytes*(pBmp->bmHeight-l-1), pBmp->bmWidthBytes, 1, fp);
	}
	
	fclose(fp);
	#endif
    return pBmp;
}

int show_winbmp(PBITMAP pBitmap, int x, int y) 
{ 
    if (!pBitmap) 
		return -1; 
	
   // print_bitmap_info(pBitmap); 

    int i, j, bytespix; 
	RGBQUAD * palette;
	BYTE *dest, *src;
    
	PSCREEN screen = fb_get_screen();
	bytespix = screen->bits_per_pixel / 8;

	switch(pBitmap->bmBitsPixel)
	{
		case 1:
		case 2:
		case 4:
			break;
		case 8:	
    		if(pBitmap->bmPalettes)
			{
    			if(screen->visual == FB_VISUAL_PSEUDOCOLOR )
				{
							RGBQUAD* pal;
							RGBQUAD* palette = (RGBQUAD*)pBitmap->bmPalettes;
							struct fb_cmap map;
							map.start = 0;
  						    //map.len = 1 << screen->bits_per_pixel;
  						    map.len = pBitmap->bmPalNum;
							map.red = (uint16*)malloc(pBitmap->bmPalNum*sizeof(uint16));
							if(!map.red) return -1;
						  	map.green = (uint16*)malloc(pBitmap->bmPalNum*sizeof(uint16));
							if(!map.green) return -1;
						  	map.blue = (uint16*)malloc(pBitmap->bmPalNum*sizeof(uint16));
							if(!map.blue) return -1;
							map.transp = NULL;
							int n;
							for(n=0; n<pBitmap->bmPalNum; n++)
							{
								pal = palette+n;	
								map.red[n] = pal->rgbRed<<8|pal->rgbRed;
								map.green[n] = pal->rgbGreen<<8|pal->rgbGreen;
								map.blue[n] = pal->rgbBlue<<8 | pal->rgbBlue;
							}
							fb_set_cmap(&map);
							free(map.red);
							free(map.green);
							free(map.blue);
				}
			}

			switch(screen->bits_per_pixel)
    		{
				case 8:	
					for (i=0; i<pBitmap->bmHeight; i++) 
			   		{ 
						dest = screen->start + (i + y)* screen->pitch + x * bytespix; 
						src = pBitmap->bmBits + (pBitmap->bmHeight-i-1) * pBitmap->bmWidthBytes;

						for (j=0; j<pBitmap->bmWidth; j++) 
						{
							*(dest+j) = *(src+j);
						}
   					}	 
					break;
				case 16:
					for (i=0; i<pBitmap->bmHeight; i++) 
			   		{ 
						LONG color;
						RGBQUAD * rgba;
						UINT * idest;
						dest = screen->start + (i + y)* screen->pitch + x * bytespix; 
						idest = (UINT *)dest; 
						src	= pBitmap->bmBits + (pBitmap->bmHeight-i-1) * pBitmap->bmWidthBytes;
						palette = (RGBQUAD * )pBitmap->bmPalettes;
						for (j=0; j<pBitmap->bmWidth; j++) 
						{
							rgba = &palette[*(src+j)];	
							color = (rgba->rgbBlue >>3) | ((rgba->rgbGreen >> 2) << 5) | ((rgba->rgbRed >> 3) << 11);
							*(idest+j) = (UINT)color;	
						}
   					}	 
					break;
				case 24:
					for (i=0; i<pBitmap->bmHeight; i++) 
			   		{ 
						RGBQUAD * rgba;
						dest = screen->start + (i + y)* screen->pitch + x * bytespix; 
						src	= pBitmap->bmBits + (pBitmap->bmHeight-i-1) * pBitmap->bmWidthBytes;
						palette = (RGBQUAD * )pBitmap->bmPalettes;
						for (j=0; j<pBitmap->bmWidth; j++) 
						{
							rgba = &palette[*(src+j)];	
							*(dest+j*3) = rgba->rgbBlue;	
							*(dest+j*3+1) = rgba->rgbGreen;	
							*(dest+j*3+2) = rgba->rgbRed;	
						}
   					}	 
					break;
				case 32:
					for (i=0; i<pBitmap->bmHeight; i++) 
			   		{ 
						LONG color;
						RGBQUAD * rgba;
						LONG * idest;
						dest = screen->start + (i + y)* screen->pitch + x * bytespix; 
						idest = (LONG *)dest; 
						src	= pBitmap->bmBits + (pBitmap->bmHeight-i-1) * pBitmap->bmWidthBytes;
						palette = (RGBQUAD * )pBitmap->bmPalettes;
						for (j=0; j<pBitmap->bmWidth; j++) 
						{
							rgba = &palette[*(src+j)];	
							color = rgba->rgbBlue | (rgba->rgbGreen << 8) | (rgba->rgbRed << 16);
							*(idest+j) = (LONG)color;	
						}
   					}	 
					break;
				default:
					break;
			}
			break;
		case 24:
			switch(screen->bits_per_pixel)
    		{
				case 8:	
					for (i=0; i<pBitmap->bmHeight; i++) 
			   		{ 
						BYTE color;
						dest = screen->start + (i + y)* screen->pitch + x * bytespix; 
						src	= pBitmap->bmBits + (pBitmap->bmHeight-i-1) * pBitmap->bmWidthBytes;
						for (j=0; j<pBitmap->bmWidth; j++) 
						{
							color = (*(src+j*3) >>5) | ((*(src+j*3+1) >> 5) << 3) | ((*(src+j*3+2) >> 6) << 6);
							*(dest+j) = (BYTE)color;	
						}
   					}	 
					break;
				case 16:
					for (i=0; i<pBitmap->bmHeight; i++) 
			   		{ 
						LONG color;
						UINT * idest;
						dest = screen->start + (i + y)* screen->pitch + x * bytespix; 
						idest = (UINT *)dest; 
						src	= pBitmap->bmBits + (pBitmap->bmHeight-i-1) * pBitmap->bmWidthBytes;
						for (j=0; j<pBitmap->bmWidth; j++) 
						{
							color = (*(src+j*3+2) >>3) | ((*(src+j*3+1) >> 2) << 5) | ((*(src+j*3) >> 3) << 11);
							*(idest+j) = (UINT)color;	
						}
   					}	 
					break;
				case 24:
					for (i=0; i<pBitmap->bmHeight; i++) 
			   		{ 
						dest = screen->start + (i + y)* screen->pitch + x * bytespix; 
						src	= pBitmap->bmBits + (pBitmap->bmHeight-i-1) * pBitmap->bmWidthBytes;
						//memset(dest, 0xff, screen->pitch);
						memcpy(dest, src, pBitmap->bmWidth * bytespix);
   					}	 
					break;
				case 32:
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	return 0; 
} 

