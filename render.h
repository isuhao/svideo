#ifndef RENDER_H
#define RENDER_H
#include "irender.h"



#define RENDER_TYPE_GDI 0
#define RENDER_TYPE_D3D 1
#define RENDER_TYPE_SDL 2

render *render_create(int type,int32_t hwnd, int pic_w, int pic_h);

#endif