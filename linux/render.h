#ifndef RENDER_H
#define RENDER_H
#include "../hvtype.h"
struct render;
typedef struct render render;
render *render_create(int32_t hwnd, int pic_w, int pic_h);
void render_draw(render *self,hvframe *frame);
void render_free(render *self);

#endif
