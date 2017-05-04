#include "render_carbox.h"
#include <windows.h>
#include <stdlib.h>
//1920*0.1
#define W 192

//1080*0.2
#define H 216

#define X 1248

#define Y 702

#define STR_W 50
#define STR_H 20


struct render_box{
	HDC dc;
	HBITMAP map;
	HDC mdc;
	RECT rec;
	RECT str_rec;
	HPEN pen;
	HBRUSH str_brush;
	POINT p[5];
	int w;
	int h;

};


struct render_box * 
render_box_create(int hwnd){
	HDC dc=GetDC((HWND)hwnd);
	if(dc==INVALID_HANDLE_VALUE){
		return NULL;
	}
	struct render_box *self=(struct render_box*)malloc(sizeof(*self));
	self->dc=dc;
	RECT face;
	GetWindowRect((HWND)hwnd,&face);
	int hw=face.right-face.left;
	int hh=face.bottom-face.top;
	self->w=hw;
	self->h=hh;
	int w=hw*0.1;
	int h=hh*0.2;
	self->rec.left=hw*0.65;
	self->rec.right=self->rec.left+w;
	self->rec.top=hh*0.65;
	self->rec.bottom=self->rec.top+h;
	self->str_rec.left=(w-STR_W)/2+self->rec.left;
	self->str_rec.top=self->rec.bottom-STR_H;
	self->str_rec.right=self->str_rec.left+STR_W;
	self->str_rec.bottom=self->str_rec.top+STR_H;
	self->pen=CreatePen( PS_DASHDOTDOT , 1 , RGB( 255 , 0 , 0 ));
	self->str_brush=CreateSolidBrush(RGB(255,0,0));
	self->mdc=CreateCompatibleDC(dc);
	self->map=CreateCompatibleBitmap(self->mdc,hw,hh);
	SelectObject(self->mdc,self->map);

	SetBkMode(self->mdc,TRANSPARENT);
	SetTextColor(self->mdc,RGB(255,255,255));
	SelectObject(self->mdc,self->pen);
    self->p[0].x=self->rec.left;
	self->p[0].y=self->rec.top;
	self->p[1].x=self->rec.right;
	self->p[1].y=self->rec.top;
	self->p[2].x=self->rec.right;
	self->p[2].y=self->rec.bottom;
	self->p[3].x=self->rec.left;
	self->p[3].y=self->rec.bottom;
	self->p[4].x=self->rec.left;
	self->p[4].y=self->rec.top;

	return self;
}

void 
render_box_draw(struct render_box *self){
	
	/*BitBlt(self->dc,
			0,0,self->w,self->h,
			self->mdc,0,0,SRCAND);*/
	Polyline(self->mdc,self->p,5);
	FillRect(self->mdc,&self->str_rec,self->str_brush);
	TextOut(self->mdc,self->str_rec.left+2,self->str_rec.top+2,"³µÅÆ¿ò",6);
	BitBlt(self->dc,
			0,0,self->w,self->h,
			self->mdc,0,0,SRCCOPY);
}


void 
render_box_destory(struct render_box *self){
	DeleteObject(self->pen);
	DeleteObject(self->str_brush);
	DeleteDC(self->dc);
	free(self);
}