#include "hvtype.h"
#include "svideo_code.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"

#include <stdlib.h>
#include <string.h>





struct svideo_code
{
	AVCodec *codec;
	AVCodecContext *cc;
	AVFrame *yuv_buf;
	AVPacket pk;
};


svideo_code * svideo_code_new(int w,int h)
{
	svideo_code *re =(svideo_code *)calloc(sizeof(svideo_code),1);
	if(re==0) return (svideo_code*)-1;
	re->codec=avcodec_find_decoder(CODEC_ID_H264);
	
	if(re->codec<0)
	{
		free(re);
		return 0;
	}
	re->cc=avcodec_alloc_context3(re->codec);
	if(re->cc<0)
	{
		goto err1;
	}
	re->cc->frame_number=1;
	re->cc->time_base.num = 1;  
	re->cc->codec_type = AVMEDIA_TYPE_VIDEO;  
	re->cc->bit_rate = 0;  
	re->cc->width = w;
	re->cc->height = h;
	if(avcodec_open2(re->cc,re->codec,NULL)<0)
		goto err1;
	re->yuv_buf=av_frame_alloc();
	if(re->yuv_buf<0) goto err1;
	return re;

err1:
	svideo_code_free(&re);
	return re;
}

void svideo_code_free( svideo_code **h){
	if(h==0) return;
	if((*h)->cc){
		avcodec_close((*h)->cc);
		avcodec_free_context(&(*h)->cc);
	}
	if((*h)->yuv_buf)
		av_frame_free(&(*h)->yuv_buf);
	*h=0;
}


int svideo_code_decode(svideo_code *h,uint8_t * src,uint32_t src_size,uint8_t *dest,uint32_t dest_size)
{
	if(h==0) return -1;
	h->pk.data=src;
	h->pk.size=src_size;
	int result,iy,iu;
	avcodec_decode_video2(h->cc,h->yuv_buf,&result,&h->pk);
	if(result>0) {


		iy=h->yuv_buf->height*abs(h->yuv_buf->linesize[0]);
		iu=h->yuv_buf->height/2*abs(h->yuv_buf->linesize[1]);
		memcpy(dest,h->yuv_buf->data[0],iy);
		memcpy(dest+iy,h->yuv_buf->data[1],iu);
		memcpy(dest+iy+iu,h->yuv_buf->data[2],iu);

		
		return 0;
	}
	return -1;
}
