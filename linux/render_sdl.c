#include <stdlib.h>
#include <string.h>
#include "render.h"
#include "../sdl/include/SDL.h"
#include "../hvpfconvert.h"

struct render
{
	int32_t hwnd;
	SDL_Window *window;
	SDL_Renderer *render;
	SDL_Texture *texture;
};

render *render_create(int hwnd,int pic_w,int pic_h)
{
	render *result=(render *)malloc(sizeof(*result));
	result->hwnd=hwnd;
	result->window=SDL_CreateWindowFrom((void*)hwnd);
	result->render=SDL_CreateRenderer(result->window,-1,SDL_RENDERER_SOFTWARE);
	result->texture=SDL_CreateTexture(result->render,SDL_PIXELFORMAT_IYUV,SDL_TEXTUREACCESS_STREAMING,pic_w,pic_h);
	return result;
}

void render_draw(render *self,hvframe *frame)
{
	if(self->texture==NULL) return;
	SDL_UpdateYUVTexture(self->texture,NULL,frame->y,frame->ypitch,
			frame->u,frame->uvpitch,frame->v,frame->uvpitch);
	SDL_RenderCopy(self->render,self->texture,NULL,NULL);
	SDL_RenderPresent(self->render);

}

void render_free(render *self)
{
	SDL_RenderClear(self->render);
	SDL_DestroyTexture(self->texture);
	SDL_DestroyRenderer(self->render);
	//SDL_DestroyWindow(self->window);
	free(self);
}
