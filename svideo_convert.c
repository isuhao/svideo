#include "hvtype.h"
#include "svideo_convert.h"

#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavutil/frame.h"

#include <assert.h>

#define HIGHT(d) (d&0xffff0000)>>16
#define LOWER(d) d&0x0000ffff
#define COM(a,b) (a<<16)|(b&0xffff)
#define PF2AVPF(pft,avt) switch (pft){\
case PF_RGB24:\
    *avt= AV_PIX_FMT_RGB24;\
	break;\
case PF_BGR24:\
	*avt=  AV_PIX_FMT_BGR24;\
	break;\
case PF_YUV420P:\
	*avt=  AV_PIX_FMT_YUV420P;\
	break;\
}


struct svideo_convert
{ 
	struct SwsContext *sc;
	AVFrame *sf;
	AVFrame *df;
	enum AVPixelFormat st;
	enum AVPixelFormat dt;
	int ssize;
	int dsize;
};

svideo_convert *svideo_convert_new(int sw,int sh,
							 int dw,int dh,
							 enum pic_formart srctype,
							 enum pic_formart desttype)
{
	  int rn=-1;
      svideo_convert *re=(svideo_convert*)malloc(sizeof(svideo_convert));
	  if(re==0) goto err0;
	  PF2AVPF(srctype,&re->st);
	  PF2AVPF(desttype,&re->dt);
	  re->sc=sws_getContext(sw,sh,re->st,dw,dh,re->dt,0,NULL,NULL,NULL);
	  if(re->sc<0) goto err0;
	  re->sf=av_frame_alloc();
	  if(re->sf<0) goto err1;
	  re->df=av_frame_alloc();
	  if(re->df<0) goto err2;
	  re->ssize=COM(sw,sh);
	  re->dsize=COM(dw,dh);
	  return re;
err2:
	av_frame_free(&re->sf);
err1:
	sws_freeContext(re->sc);
err0:
	free(re);
	return (svideo_convert*)rn;
};
void svideo_convert_free(svideo_convert **h)
{
	if(h==0) return;
	sws_freeContext((*h)->sc);
	free(*h);
	*h=0;
}
int svideo_convert_convert(svideo_convert *h,uint8_t *src,uint8_t *dest)
{
	avpicture_fill((AVPicture*)h->sf,src,h->st,HIGHT(h->ssize),LOWER(h->ssize));
	avpicture_fill((AVPicture*)h->df,dest,h->dt,HIGHT(h->dsize),LOWER(h->dsize));
	return sws_scale(h->sc,h->sf->data,h->sf->linesize,0,LOWER(h->ssize),h->df->data,h->df->linesize)==1? 0:-1;

}

int svideo_convert_convert2(svideo_convert *h,hvframe *src,uint8_t *dest)
{
	avpicture_fill((AVPicture*)h->df,dest,h->dt,HIGHT(h->dsize),LOWER(h->dsize));
	h->sf->data[0]=src->y;
	h->sf->data[1]=src->u;
	h->sf->data[2]=src->v;
	h->sf->linesize[0]=src->ypitch;
	h->sf->linesize[1]=h->sf->linesize[2]=src->uvpitch;
	return sws_scale(h->sc,h->sf->data,h->sf->linesize,0,LOWER(h->ssize),h->df->data,h->df->linesize)==1? 0:-1;
}
uint32_t svideo_convert_get_size(int w,int h,enum pic_formart pt)
{
	enum AVPixelFormat t;
    PF2AVPF(pt,&t);
	return avpicture_get_size(t,w,h);
};
