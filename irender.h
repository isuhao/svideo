#ifndef IRENDER_H
#define IRENDER_H
#include "hvtype.h"
struct render{
	int hwnd;
	int (*draw)(struct render *self,hvframe *frame);
	void (*destory)(struct render **self);
};
typedef struct render render;

render *gdi_new(int32_t hwnd,int pic_w,int pic_h);
render *d3d_new(int hwnd,int pic_w,int pic_h);
render *sdl_new(int hwnd,int pic_w,int pic_h);

#endif