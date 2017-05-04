#ifndef WINAPI_H
#define WINAPI_H
#include <windows.h>
#include <objbase.h>
#include <d3d9.h>
typedef HGDIOBJ (WINAPI *sGetStockObject)(int i);
typedef int (WINAPI *sSetStretchBltMode)(HDC dc,int mode);
typedef int (WINAPI *sStretchDIBits)
	       ( HDC hdc,  
		     int xDest, 
			 int yDest,  
			 int DestWidth, 
			 int DestHeight,  
			 int xSrc,  
			 int ySrc, 
			 int SrcWidth,  
			 int SrcHeight,
			 const void * lpBits,  
			 const BITMAPINFO * lpbmi,  
			 UINT iUsage,  
			 DWORD rop);
typedef IDirect3D9*(WINAPI *sDirect3DCreate9)(UINT version);

extern sGetStockObject pGetStockObject;
extern sSetStretchBltMode pSetStretchBltMode;
extern sStretchDIBits pStretchDIBits;
extern sDirect3DCreate9 pDirect3DCreate9;


int hv_winapi_init(void);

extern int sdk_init;

#define CHECK_SDK_INIT if(sdk_init==0) return NULL
#endif