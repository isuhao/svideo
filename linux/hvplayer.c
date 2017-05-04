
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "../hvplayer.h"
#include "vad.h"
#include "h264_recv.h"
INCLUDE_BEGIN
#include <libavcodec/avcodec.h>
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
INCLUDE_END

pthread_mutex_t lock;

struct hvplayer
{
	playend_cb end;
	int32_t hwnd;
	int32_t flage;
	char *url;
	enum player_state ste;
	clock_t pretm;
	bool play;
	int frame;
	int w;
	int h;
        int recv_protocol;
};

struct decode_context{
	AVCodecContext *cc;
	AVCodec *codec;
	hvplayer *p;

};

#define  TIMEOUT_S 6

int timeoutcheck(void *p)
{
	hvplayer *h=(hvplayer*)p;
	if(h->flage==0) return 1;
	if(h->ste==PLAYER_CONNECTING)
	{
		clock_t ctm=clock();
		int s=(ctm-h->pretm)/CLOCKS_PER_SEC;
		if(s>=TIMEOUT_S)
			return 1;		
	}
	return 0;
}


static void _do(hvplayer*h)
{
	AVCodec *codec=NULL;
	AVCodecContext *cc=NULL;
	AVPacket pk={0};
	AVFormatContext *afc=NULL;
	int vindex=-1;
	afc=avformat_alloc_context();
	if(afc==0){
		goto err;
	}
	afc->interrupt_callback.callback=timeoutcheck;
	afc->interrupt_callback.opaque=h;
	AVDictionary *dir=NULL;
	char *k1="stimeout";
	char *v1="10";
	char *k2="rtsp_transport";
	char *v2="udp";
	//char *k3="b";
	//char *v3="1500";
	char *k4="max_delay";
	char *v4="50000";
	int r=av_dict_set(&dir,k1,v1,0);
	r=av_dict_set(&dir,k2,v2,0);
	//r=av_dict_set(&dir,k3,v3,0);
	av_dict_set(&dir,k4,v4,0);
	if(avformat_open_input(&afc,h->url,NULL,&dir)) {
		goto err;
	}
	if(avformat_find_stream_info(afc,NULL)<0) {
		goto err;
	}
	for(int i=0;i<(int)afc->nb_streams;i++)
	{
		if(afc->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			vindex=i;
			break;
		}
	}
	if(vindex==-1){
		goto err;
	}
	cc=afc->streams[vindex]->codec;
	codec=avcodec_find_decoder(cc->codec_id);//AV_CODEC_ID_H264);
	//cc=avcodec_alloc_context3(codec);
	if(vad_open(cc)<0)  goto err;
	if(codec==0) goto end;
	if(avcodec_open2(cc,codec,NULL)<0) {
		goto err;
	}
	int dr;
	hvframe frame;
	AVFrame *yuv_buf=av_frame_alloc();
	if(yuv_buf==0) {
		goto end;
	}
	h->ste=PLAYER_PLAYING;
	while (h->flage&&av_read_frame(afc,&pk)>=0)
	{
		pthread_mutex_lock(&lock);
		vad_decode(cc,pk.data,pk.size,h->w,h->h,h->hwnd);
		av_free_packet(&pk);
		pthread_mutex_unlock(&lock);
	}
end:
	printf("connect error\n");
	if(yuv_buf) av_frame_free(&yuv_buf);
err:
	printf("connect error\n");
	if(cc) avcodec_close(cc);
	if(afc){
		avformat_close_input(&afc);
		avformat_free_context(afc);
	}
}


static void
va_decode(char *data,int size,void *ud){
     struct decode_context *ctx=ud;
     vad_decode(ctx->cc,data,size,ctx->p->w,ctx->p->h,ctx->p->hwnd);
}


static void *
do2(hvplayer *h){
   AVCodec *codec=NULL;
   AVCodecContext *cc=NULL;
   codec=avcodec_find_decoder(AV_CODEC_ID_H264);
   cc=avcodec_alloc_context3(codec);
   if(vad_open(cc)<0) goto end;
   if(avcodec_open2(cc,codec,NULL)<0) goto end;
   struct decode_context ctx;
   ctx.cc=cc;
   ctx.codec=codec;
   ctx.p=h;
   ReceiveVideo(h->url,va_decode,&ctx);
end:
   if(cc) avcodec_close(cc);

}

static void*
play_loop(void* p)
{
	hvplayer *h=(hvplayer*)p;
	while(h->flage)
	{
		h->ste=PLAYER_CONNECTING;
		h->pretm=clock();
		printf("connect \n");
                if(h->recv_protocol==0)
		_do(h);
                else
		 do2(h);
	}
	if(h->end) h->end(h);
	free(h->url);
	free(h);
	return NULL;
}


hvplayer *hvplayer_new(int32_t hwnd, const char *url)
{
	if(hwnd==0||url==NULL) return NULL;
	hvplayer *re=(hvplayer*)calloc(1,sizeof(hvplayer));
	re->url=strdup(url);
	re->ste=PLAYER_OFF;
	re->hwnd=hwnd;
	return re;
}

int hvplayer_play(hvplayer *self)
{
	if(self->play) return 0;
	self->flage=1;
/*	pthread_t pid;
	pthread_create(&pid, NULL, play_loop, self);
	self->play=true;*/
    pid_t pid=fork();
	if(pid<0) return -1;
	if(pid==0){
		play_loop(self);
	}
	return 0;
}

int
hvplayer_play2(hvplayer *self){
    if(self->play) return 0;
	self->flage=1;
        self->recv_protocol=1;
//	pthread_t pid;
//	pthread_create(&pid, NULL, play_loop, self);
//	self->play=true;
    pid_t pid=fork();
	if(pid<0) return -1;
	if(pid==0){
		hvdevicevideo_init();
		play_loop(self);
	}
	return 0;
}


void hvplayer_close(hvplayer *h)
{
	h->flage=0;
	h->pretm=0;
}
void hvplayer_set_endcb(hvplayer *h,playend_cb cb)
{
	h->end=cb;
}

int hvplayer_getstate(hvplayer *h)
{
	return (int)h->ste;
}

int hvdevicevideo_init(void){
	avcodec_register_all();
	av_register_all();
	avformat_network_init();
	return 0;
}

void hvplayer_set_window(hvplayer *h,int width,int height){
	h->w=width;
	h->h=height;
}

int hvplayer_get_frame(hvplayer *h)
{
	return h->frame;
}


