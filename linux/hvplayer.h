#ifndef HVPLAYER_H
#define HVPLAYER_H

#define HVPLAYER_OP_ADDR 0
#define HVPLAYER_OP_HWND 1
#define HVPLAYER_OP_WIDTH 2
#define HVPLAYER_OP_HEIGHT 3
#define HVPLAYER_OP_PROTOCOL 4

void hvdevicevideo_init();

int hvplayer_create();
int hvplayer_set_option(int id,int op,char *value);
int hvplayer_play(int id);
int hvplayer_close(int id);
void hvplayer_closeall();
#endif
