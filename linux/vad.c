#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "vad.h"
#include "vaapi.h"
#include "my_utils.h"
#include "libavcodec/vaapi.h"
# include <libavformat/avformat.h>

struct vad{
	struct vaapi_context  *mva;
	VAAPIContext *vaapi;
	Display *dispay;
	AVPacket pk;
	AVFrame *fm;
};

static enum AVPixelFormat
get_format(struct AVCodecContext *avctx,const enum AVPixelFormat *fmt){
	int i, profile;
	printf("get format\n");
	struct vad * ctx = (struct vad *)(avctx->opaque);
	printf("get format ctx:%p\n",ctx);
	for (i = 0; fmt[i] != PIX_FMT_NONE; i++)
	{
		if (fmt[i] != PIX_FMT_VAAPI_VLD)
			continue;
		switch (avctx->codec_id)
		{
			case CODEC_ID_MPEG2VIDEO:
				profile = VAProfileMPEG2Main;
				break;
			case CODEC_ID_MPEG4:
			case CODEC_ID_H263:
				profile = VAProfileMPEG4AdvancedSimple;
				break;
			case CODEC_ID_H264:
				profile = VAProfileH264High;
				break;
			case CODEC_ID_WMV3:
				profile = VAProfileVC1Main;
				break;
			case CODEC_ID_VC1:
				profile = VAProfileVC1Advanced;
				break;
			default:
				profile = -1;
				break;
		}
		if (profile >= 0)
		{
			if (vaapi_init_decoder(ctx->vaapi,(VAProfile) profile, VAEntrypointVLD,
						avctx->width, avctx->height) == 0)
			{
				printf("vaapi init decode ok\n");
				struct vaapi_context* mva = (struct vaapi_context*) malloc(
						sizeof(struct vaapi_context));
				mva->display = ctx->vaapi->display;
				mva->config_id = ctx->vaapi->config_id;
				mva->context_id = ctx->vaapi->context_id;

				mva->pic_param_buf_id = 0;
				mva->iq_matrix_buf_id = 0;
				mva->bitplane_buf_id = 0;
				mva->slice_buf_ids = 0;
				mva->n_slice_buf_ids = 0;
				mva->slice_buf_ids_alloc = 0;
				mva->slice_params = ctx->vaapi->slice_params;
				mva->slice_param_size = ctx->vaapi->slice_param_size;
				mva->slice_params_alloc = ctx->vaapi->slice_params_alloc;
				//mva->slice_count = 0;
				mva->slice_data = ctx->vaapi->slice_data;
				mva->slice_data_size = ctx->vaapi->slice_data_size;
				//memset(mva->slice_data, 0, mva->slice_data_size);
				ctx->mva=mva;
				TRACE(INFO,"config id %d, context_id %d, addr %p pic_param_buf_id %d,iq_matrix_buf_id %d,bitplane_buf_id %d\n",
						ctx-> vaapi->config_id, ctx->vaapi->context_id, ctx->vaapi,mva->pic_param_buf_id,mva->iq_matrix_buf_id,mva->bitplane_buf_id);
				avctx->hwaccel_context = mva;

				TRACE(INFO,"========hwaccel_context addr %p\n",mva);
				TRACE(INFO,"========vaapi addr %p\n",ctx->vaapi);
				TRACE(INFO,"======== addr surface_id%p\n",ctx->vaapi->surface_id);
				return fmt[i];
			}
		}
	}
	return PIX_FMT_NONE;
}

static int
get_buffer(struct AVCodecContext *avctx, AVFrame *pic){
	//VAAPIContext * const vaapi = vaapi_get_context();
	struct vad * ctx = (struct vad *)(avctx->opaque);
	//void *surface = (void *) (uintptr_t) pThis->vaapi->surface_id;
	void *surface = (void *) (uintptr_t) CreateSurface(ctx->vaapi);

	pic->type = FF_BUFFER_TYPE_USER;
	pic->data[0] = (uint8_t*)surface;
	pic->data[1] = NULL;
	pic->data[2] = NULL;
	pic->data[3] = (uint8_t*)surface;
	pic->linesize[0] = 0;
	pic->linesize[1] = 0;
	pic->linesize[2] = 0;
	pic->linesize[3] = 0;

	pic->opaque = surface;
	return 0;
}

	static void
release_buffer(struct AVCodecContext *avctx, AVFrame *pic)
{
	struct vad * ctx = (struct vad *)(avctx->opaque);
	DestroySurface(ctx->vaapi,(VASurfaceID)pic->opaque);
	pic->data[0] = NULL;
	pic->data[1] = NULL;
	pic->data[2] = NULL;
	pic->data[3] = NULL;
}

static void
ffmpeg_init_context(AVCodecContext *avctx){
	avctx->flags|= CODEC_FLAG_EMU_EDGE;
	avctx->thread_count    = 1;
	avctx->get_format = get_format;
	avctx->get_buffer = get_buffer;
	avctx->reget_buffer = get_buffer;
	avctx->release_buffer = release_buffer;
	avctx->draw_horiz_band = NULL;
	avctx->slice_flags     = SLICE_FLAG_CODED_ORDER|SLICE_FLAG_ALLOW_FIELD;
	printf("ffmpeg fill success\n");
}

int vad_open(AVCodecContext *avctx){
	struct vad *ctx=(struct vad*)malloc(sizeof(*ctx));
	ctx->vaapi=(VAAPIContext*)malloc(sizeof(VAAPIContext));
	memset(ctx->vaapi,0,sizeof(VAAPIContext));
	ctx->dispay=XOpenDisplay(NULL);
	if(ctx->dispay<=0){
		fprintf(stderr,"create display error\n");
		return -1;
	}
	VADisplay dpy=vaGetDisplay(ctx->dispay);
	if( vaapi_init(ctx->vaapi,dpy)<0){
		fprintf(stderr,"vaapi initi failed\n");
		return -1;
	}

	avctx->opaque=ctx;
	ffmpeg_init_context(avctx);
	ctx->fm=av_frame_alloc();
	av_init_packet(&ctx->pk);
	printf("vaapi initit success\n");
}

int
vad_decode(AVCodecContext *avctx,char *buf,int n,int w,int h,int hwnd){
	struct vad * ctx = (struct vad *)(avctx->opaque);
	static int i;
	int got_picture = 0;
	int len = 0;

	av_init_packet(&ctx->pk);
	ctx->pk.data = (uint8_t*) buf;
	ctx->pk.size = n;

	len = avcodec_decode_video2(avctx, ctx->fm, &got_picture,&ctx->pk);

	if (got_picture)
	{

		PutSurface(ctx->vaapi,(VASurfaceID)ctx->fm->opaque,hwnd,w,h); 
	}

	return 0;
}
