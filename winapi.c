#include "winapi.h"

 sGetStockObject pGetStockObject;
 sSetStretchBltMode pSetStretchBltMode;
 sStretchDIBits pStretchDIBits;
 sDirect3DCreate9 pDirect3DCreate9;

 int sdk_init=0;
int hv_winapi_init(void){
	HMODULE user32_module;
	user32_module=GetModuleHandleA("gdi32.dll");
	if(user32_module==NULL){
		return -1;
	}
	pGetStockObject=(sGetStockObject)GetProcAddress(user32_module,"GetStockObject");
	if(pGetStockObject==NULL) return -1;
	pSetStretchBltMode=(sSetStretchBltMode)GetProcAddress(user32_module,"SetStretchBltMode");
	if(pSetStretchBltMode==NULL) return -1;
	pStretchDIBits=(sStretchDIBits)GetProcAddress(user32_module,"StretchDIBits");
	if(pStretchDIBits==NULL) return -1;
	return 0;
}



