#include <stdlib.h>
#include <windows.h>
#include "svideo_convert.h"
#include "irender.h"
#include "winapi.h"

struct render_gdi
{
	render base;
	BITMAPINFO  bmp;
	HDC dc;
	svideo_convert *convert;
	uint8_t *buf;
	RECT rec;
};


void gdi_free(render **h)
{
	if((*h)==0)return;
	struct render_gdi *self=(struct render_gdi*)(*h);
    FillRect(self->dc,&self->rec,(HBRUSH)(pGetStockObject(BLACK_BRUSH)));
	ReleaseDC((HWND)self->base.hwnd,self->dc);
	svideo_convert_free(&self->convert);
	free(*h);
	*h=0;
}

int gdi_draw(render *h,hvframe *frame)
{
	struct render_gdi *self=(struct render_gdi*)h;
	svideo_convert_convert2(self->convert, frame, self->buf);
	pSetStretchBltMode(self->dc, STRETCH_HALFTONE);
	pStretchDIBits(self->dc, 0, 0, self->rec.right, self->rec.bottom, 0, 0,
		self->bmp.bmiHeader.biWidth, frame->h, self->buf, &self->bmp, DIB_RGB_COLORS, SRCCOPY);
		return 0;
}

render *gdi_new(int32_t hwnd,int pic_w,int pic_h)
{
	struct render_gdi *re = (struct render_gdi *)calloc(1,sizeof(*re));
	re->convert = svideo_convert_new(pic_w, pic_h, pic_w, pic_h, PF_YUV420P, PF_BGR24);
	re->dc = GetDC((HWND)hwnd);
	re->base.hwnd = hwnd;
	GetClientRect((HWND)re->base.hwnd, &re->rec);
	re->buf = (uint8_t*)malloc(svideo_convert_get_size(pic_w, pic_h, PF_RGB24));
	re->bmp.bmiHeader.biBitCount = 24;
	re->bmp.bmiHeader.biClrImportant = BI_RGB;
	re->bmp.bmiHeader.biClrUsed = 0;
	re->bmp.bmiHeader.biCompression = 0;
	re->bmp.bmiHeader.biHeight = -pic_h;
	re->bmp.bmiHeader.biWidth = pic_w;
	re->bmp.bmiHeader.biPlanes = 1;
	re->bmp.bmiHeader.biSize = sizeof(re->bmp.bmiHeader);
	re->bmp.bmiHeader.biSizeImage = pic_w*pic_h * 3;
	re->bmp.bmiHeader.biXPelsPerMeter = 0;
	re->bmp.bmiHeader.biYPelsPerMeter = 0;
	re->base.destory=gdi_free;
	re->base.draw=gdi_draw;
	return (render *)re;
}
