#include "render.h"

render *render_create(int type,int32_t hwnd, int pic_w, int pic_h){
	if(type==RENDER_TYPE_D3D)
		return d3d_new(hwnd,pic_w,pic_h);
	else
		return gdi_new(hwnd,pic_w,pic_h);
}