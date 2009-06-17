/*
   Common data types
   Copyright (C) He Zhe 2003-2006
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef _TYPES_H
#define _TYPES_H
typedef unsigned char uint8;
typedef signed char sint8;
typedef unsigned short uint16;
typedef signed short sint16;
typedef unsigned int uint32;
typedef signed int sint32;

typedef unsigned long       DWORD;
typedef long                LONG;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT              *PFLOAT;
typedef BOOL               *PBOOL;
typedef BOOL               *LPBOOL;
typedef BYTE               *PBYTE;
typedef BYTE               *LPBYTE;
typedef int                *PINT;
typedef int                *LPINT;
typedef WORD               *PWORD;
typedef WORD              *LPWORD;
typedef long              *LPLONG;
typedef DWORD            *PDWORD;
typedef DWORD             *LPDWORD;
typedef void              *LPVOID;

typedef short                 INT;
typedef unsigned short        UINT;
typedef unsigned short*		  PUINT;


#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#ifndef True
#define True  1
#define False 0
#endif

#endif
