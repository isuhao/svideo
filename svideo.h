#ifndef SVIDEO_H
#define SVIDEO_H

void svideo_init();
int svideo_play(const char *url,unsigned long long hwnd,const char * protocol);
void svideo_close_all();

#endif
