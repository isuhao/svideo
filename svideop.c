#include <string.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "winapi.h"
#include "svideo_rtsp.h"
#include "render.h"
#include "render_carbox.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"

#define  TIMEOUT_S 6
uint64_t _hwnd;
char *_url;
char *_protocol;
time_t _live;

static int
timeout(void *p){
	return time(0) - _live > 6;
}

static void
play(){
	AVCodec *codec = NULL;
	AVCodecContext *cc = NULL;
	AVPacket pk = { 0 };
	AVFormatContext *afc = NULL;
	int vindex = -1;
	afc = avformat_alloc_context();
	if (afc == 0){
		goto err;
	}
	_live = time(0);
	afc->interrupt_callback.callback = timeout;
	AVDictionary *dir = NULL;
	char *k1 = "stimeout";
	char *v1 = "10";
	char *k2 = "rtsp_transport";
	char *v2 = _protocol;
	char *k4 = "max_delay";
	char *v4 = "50000";
	int r = av_dict_set(&dir, k1, v1, 0);
	r = av_dict_set(&dir, k2, v2, 0);
	av_dict_set(&dir, k4, v4, 0);
	if (avformat_open_input(&afc, _url, NULL, &dir)) {
		goto err;
	}
	if (avformat_find_stream_info(afc, NULL) < 0) {
		goto err;
	}
	for (int i = 0; i < (int)afc->nb_streams; i++)
	{
		if (afc->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			vindex = i;
			break;
		}
	}
	if (vindex == -1){
		goto err;
	}
	cc = afc->streams[vindex]->codec;
	codec = avcodec_find_decoder(cc->codec_id);
	if (codec == 0) goto end;
	if (avcodec_open2(cc, codec, NULL) < 0) {
		goto err;
	}
	int dr;
	hvframe frame;
	render *render = render_create(RENDER_TYPE_D3D, _hwnd, cc->coded_width, cc->coded_height);
	if (render == NULL)
	{
		render = render_create(RENDER_TYPE_GDI, _hwnd, cc->coded_width, cc->coded_height);
		if (render == NULL){
			goto end;
		}
	}
	AVFrame *yuv_buf = av_frame_alloc();
	if (yuv_buf == 0) {
		goto end;
	}
	while (av_read_frame(afc, &pk) >= 0)
	{
		if (pk.stream_index == vindex)
		{
			if(avcodec_decode_video2(cc, yuv_buf, &dr, &pk) < 0){break;}
			if (dr > 0)
			{
				frame.h = cc->coded_height;
				frame.w = cc->coded_width;
				frame.y = yuv_buf->data[0];
				frame.u = yuv_buf->data[1];
				frame.v = yuv_buf->data[2];
				frame.ypitch = yuv_buf->linesize[0];
				frame.uvpitch = yuv_buf->linesize[1];
				if (render->draw(render, &frame) > 0)
				{
					render->destory(&render);
					render = render_create(RENDER_TYPE_D3D, _hwnd, cc->coded_width, cc->coded_height);
					if(render == NULL) {break;}
				}
				_live = time(0);
			}
		}
		av_free_packet(&pk);
	}
end:
	if (yuv_buf) av_frame_free(&yuv_buf);
	if (render) render->destory(&render);
err:
	if (cc) avcodec_close(cc);
	if (afc){
		avformat_close_input(&afc);
		avformat_free_context(afc);
	}
}

int
main(int argc,char **argv){
	if(argc < 4 ){
		fprintf(stderr,"must given three argusment <url> <hwnd> <protocol>");
		return -1;
	}
	if(sscanf(argv[2],"%llu",&_hwnd) <= 0 ) return -1;
	_url = argv[1];
	_protocol = argv[3];
	if(strcmp(_protocol,"tcp") != 0 && strcmp(_protocol,"udp") != 0) return -1;
	if (hv_winapi_init()) return -1;
	avcodec_register_all();
	av_register_all();
	avformat_network_init();
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	play();
	return 0;
}

