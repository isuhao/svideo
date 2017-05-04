#ifndef RENDER_BOX_H
#define RENDER_BOX_H


struct render_box;

struct render_box * render_box_create(int hwnd);
void render_box_draw(struct render_box *);
void render_box_destory(struct render_box *);


#endif