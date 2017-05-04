#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include "h264_recv.h"
#include "vad.h"

struct player_arg{
	char *addr;
	uint32_t hwnd;
	int w;
	int h;
	int protocol;
};

struct decode_context{
	AVCodecContext *cc;
	AVCodec *codec;
	struct player_arg *parg;
};

static void
parser_arg(struct player_arg *obj,char **args){
	obj->addr=args[1];
	sscanf(args[2],"%lu",&obj->hwnd);
	sscanf(args[3],"%d",&obj->w);
	sscanf(args[4],"%d",&obj->h);
	sscanf(args[5],"%d",&obj->protocol);
}

static void
va_decode(char *data,int size,void *ud){
	struct decode_context *ctx=ud;
	vad_decode(ctx->cc,
			   data,
			   size,
			   ctx->parg->w,
			   ctx->parg->h,
			   ctx->parg->hwnd);


}

static void
snt_h264_play(struct player_arg *arg){
	AVCodec *codec=NULL;
	AVCodecContext *cc=NULL;
	codec=avcodec_find_decoder(AV_CODEC_ID_H264);
	if(codec==NULL){
		fprintf(stderr,"find decode error\n");
		return;
	}
	cc=avcodec_alloc_context3(codec);
	if(cc==NULL){
		fprintf(stderr,"alloc decode context error\n");
		return;
	}
	if(vad_open(cc)<0) goto end;
	if(avcodec_open2(cc,codec,NULL)<0) goto end;
	struct decode_context ctx;
	ctx.parg=arg;
	ctx.cc=cc;
	ctx.codec=codec;
	ReceiveVideo(arg->addr,va_decode,&ctx);
end:
	if(cc) avcodec_close(cc);



}


static void
rtsp_play(struct player_arg *arg){
	AVCodec *codec=NULL;
	AVCodecContext *cc=NULL;





}

static void 
print_arg(struct player_arg *arg){
	printf("arg---------------------\n");
	printf("addr: %s\n",arg->addr);
	printf("hwnd: %lu\n",arg->hwnd);
	printf("width: %d\n",arg->w);
	printf("height: %d\n",arg->h);
	printf("protocol: %d" ,arg->protocol);


}



int
main(int argc,char **args){
	struct player_arg arg;
	memset(&arg,0,sizeof(arg));
	parser_arg(&arg,args);
	printf("hvplayer create\n");
	print_arg(&arg);
	avcodec_register_all();
	av_register_all();
	avformat_network_init();
	snt_h264_play(&arg);
	printf("unnow protocl\n");

}
