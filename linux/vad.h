#ifndef VAD_H
#define VAD_H
#include <libavcodec/avcodec.h>
int vad_open(AVCodecContext *avctx);
void vad_close(AVCodecContext *avctx);
int vad_decode(AVCodecContext *avctx,char *buf,int n,int w,int h,int hwnd);


#endif // VAD_H
