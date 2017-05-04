
#include <stdlib.h>
#include "hvtype.h"
#include "irender.h"
#include "winapi.h"

struct render_d3d
{
	render base;
	IDirect3D9 *d3d;
	IDirect3DDevice9 *d3d_dev;
	IDirect3DSurface9 *surface;
	RECT rec;
};

int d3d_draw(render *h,hvframe *frame);
void render_free(render **h);

render *d3d_new(int hwnd,int pic_w,int pic_h)
{
	struct render_d3d *result=(struct render_d3d *)malloc(sizeof(*result));
	result->base.hwnd=hwnd;
	result->d3d=Direct3DCreate9(D3D_SDK_VERSION);
	if (result->d3d == NULL)
	{
		return NULL;
	}
	D3DPRESENT_PARAMETERS d3dpp; 
	memset(&d3dpp,0,sizeof(d3dpp));
	d3dpp.Windowed=TRUE;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat=D3DFMT_UNKNOWN;
	GetClientRect((HWND)result->base.hwnd,&result->rec);
	HRESULT re=IDirect3D9_CreateDevice(result->d3d,D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,(HWND)result->base.hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&result->d3d_dev);
	if(FAILED(re)) return NULL;
	re=IDirect3DDevice9_CreateOffscreenPlainSurface(result->d3d_dev,pic_w,pic_h,
		(D3DFORMAT)MAKEFOURCC('Y','V','1','2'),
		D3DPOOL_DEFAULT,&result->surface,NULL);
	if(FAILED(re)) return NULL;
	result->base.draw=d3d_draw;
	result->base.destory=render_free;
	return (render *)result;
}

int d3d_draw(render *h,hvframe *frame)
{
	struct render_d3d *self=(struct render_d3d*)h;
	D3DLOCKED_RECT texture;
	HRESULT re=IDirect3DSurface9_LockRect(self->surface,&texture,NULL,D3DLOCK_DONOTWAIT);
	if(FAILED(re)) return;
	int uvstep=texture.Pitch/2;
	char *dest=(char*)texture.pBits;
	char *vdest=(char*)texture.pBits+frame->h*texture.Pitch;
	char *udest=(char*)vdest+frame->h/2*uvstep;
	int uvn=0;
	for(int i=0;i<frame->h;i++)
	{
		memcpy(dest,frame->y,frame->ypitch);
		frame->y+=frame->ypitch;
		dest+=texture.Pitch;
	}
	for(int i=0;i<frame->h/2;i++)
	{
		memcpy(vdest,frame->v,frame->uvpitch);
		memcpy(udest,frame->u,frame->uvpitch);
		vdest+=uvstep;
		udest+=uvstep;
		frame->v+=frame->uvpitch;
		frame->u+=frame->uvpitch;
	}
	IDirect3DSurface9_UnlockRect(self->surface);
	IDirect3DDevice9_BeginScene(self->d3d_dev);
	IDirect3DSurface9 *back_surface;
	IDirect3DDevice9_GetBackBuffer(self->d3d_dev,0,0,D3DBACKBUFFER_TYPE_MONO,&back_surface);
	IDirect3DDevice9_StretchRect(self->d3d_dev,self->surface,NULL,back_surface,&self->rec,D3DTEXF_LINEAR);
	IDirect3DDevice9_EndScene(self->d3d_dev);
	if(IDirect3DDevice9_Present(self->d3d_dev,NULL,NULL,NULL,NULL)==D3DERR_DEVICELOST){
		if(IDirect3DDevice9_TestCooperativeLevel(self->d3d_dev) ==D3DERR_DEVICENOTRESET)
		{
			return 1;
		}
	}
	return 0;
}
void render_free(render **h)
{
	if((*h)==0)return;
	struct render_d3d *self=(struct render_d3d*)(*h);
	IDirect3DDevice9_Clear(self->d3d_dev,0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
	IDirect3DDevice9_Present(self->d3d_dev,NULL,NULL,NULL,NULL);
	IDirect3DSurface9_Release(self->surface);
	IDirect3DDevice9_Release(self->d3d_dev);
	IDirect3D9_Release(self->d3d);
	free(*h);
	*h=0;
}