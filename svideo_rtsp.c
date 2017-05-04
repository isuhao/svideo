
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

struct svideo_rtsp
{
	playend_cb end;
	int32_t hwnd;
	int32_t flage;
	char *url;
	enum player_state ste;
	HANDLE thread;
	clock_t pretm;
	int play;
	int fps;

	draw_func draw_func;
	void *draw_ud;
};

#define  TIMEOUT_S 6

int timeoutcheck(void *p)
{
	svideo_rtsp *h = (svideo_rtsp*)p;
	if (h->flage == 0) return 1;
	if (h->ste == PLAYER_CONNECTING)
	{
		clock_t ctm = clock();
		int s = (ctm - h->pretm) / CLOCKS_PER_SEC;
		if (s >= TIMEOUT_S)
			return 1;
	}
	return 0;
}

static void _do(svideo_rtsp*h)
{
	AVCodec *codec = NULL;
	AVCodecContext *cc = NULL;
	AVPacket pk = { 0 };
	AVFormatContext *afc = NULL;
	int vindex = -1;
	afc = avformat_alloc_context();
	if (afc == 0){
		goto err;
	}
	afc->interrupt_callback.callback = timeoutcheck;
	afc->interrupt_callback.opaque = h;
	AVDictionary *dir = NULL;
	char *k1 = "stimeout";
	char *v1 = "10";
	char *k2 = "rtsp_transport";
	char *v2 = "udp";
	char *k4 = "max_delay";
	char *v4 = "50000";
	int r = av_dict_set(&dir, k1, v1, 0);
	r = av_dict_set(&dir, k2, v2, 0);
	av_dict_set(&dir, k4, v4, 0);
	if (avformat_open_input(&afc, h->url, NULL, &dir)) {
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
	render *render = render_create(RENDER_TYPE_D3D, h->hwnd, cc->coded_width, cc->coded_height);
	if (render == NULL)
	{
		render = render_create(RENDER_TYPE_GDI, h->hwnd, cc->coded_width, cc->coded_height);
		if (render == NULL){
			goto end;
		}
	}
	AVFrame *yuv_buf = av_frame_alloc();
	if (yuv_buf == 0) {
		goto end;
	}
	h->ste = PLAYER_PLAYING;
	
	if (h->draw_func == NULL)
	{
		while (h->flage&&av_read_frame(afc, &pk) >= 0)
		{

			if (pk.stream_index == vindex)
			{
				avcodec_decode_video2(cc, yuv_buf, &dr, &pk);
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
						render = render_create(RENDER_TYPE_D3D, h->hwnd, cc->coded_width, cc->coded_height);
					}
					h->fps = cc->framerate.num;
				}
			}
			av_free_packet(&pk);
		}
	}
	else{
		while (h->flage&&av_read_frame(afc, &pk) >= 0)
		{
			if (pk.stream_index == vindex)
			{
				avcodec_decode_video2(cc, yuv_buf, &dr, &pk);
				if (dr > 0)
				{
					frame.h = cc->coded_height;
					frame.w = cc->coded_width;
					frame.y = yuv_buf->data[0];
					frame.u = yuv_buf->data[1];
					frame.v = yuv_buf->data[2];
					frame.ypitch = yuv_buf->linesize[0];
					frame.uvpitch = yuv_buf->linesize[1];
					h->draw_func(&frame, h->draw_ud);
				}
				h->fps = cc->framerate.num;
			}
			av_free_packet(&pk);
		}
	}

end:
	if (yuv_buf) av_frame_free(&yuv_buf);
	if (render) render->destory(&render);
	//if(rbox) render_box_destory(rbox);
err:
	if (cc) avcodec_close(cc);
	if (afc){
		avformat_close_input(&afc);
		avformat_free_context(afc);
	}
}

static int play_loop(void* p)
{
	svideo_rtsp *h = (svideo_rtsp*)p;
	while (h->flage)
	{
		h->ste = PLAYER_CONNECTING;
		h->pretm = clock();
		_do(h);
		int s = TIMEOUT_S - (clock() - h->pretm) / CLOCKS_PER_SEC;
		if (s > 0) Sleep(s * 1000);
	}
	if (h->end) h->end(h);
	free(h->url);
	free(h);
	return 1;
}


svideo_rtsp *svideo_rtsp_new(int32_t hwnd, const char *url)
{
	if (hwnd == 0 || url == NULL)
	{
		return NULL;
	}
	svideo_rtsp *re = (svideo_rtsp*)calloc(1, sizeof(svideo_rtsp));
	re->url = strdup(url);
	re->fps = 0;
	re->ste = PLAYER_OFF;
	re->hwnd = hwnd;
	re->draw_func = NULL;
	return re;
}

int svideo_rtsp_play(svideo_rtsp *self)
{
	if (self->play) return 0;
	self->flage = 1;
	if ((self->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)play_loop, (LPVOID)self, 0, 0)) <= 0) return -1;
	self->play = TRUE;
	return 0;
}


void svideo_rtsp_close(svideo_rtsp *h)
{
	h->flage = 0;
	h->pretm = 0;
	CloseHandle(h->thread);
	h->thread = 0;
}
void svideo_rtsp_set_endcb(svideo_rtsp *h, playend_cb cb)
{
	h->end = cb;
}

int svideo_rtsp_getstate(svideo_rtsp *h)
{
	return (int)h->ste;
}

void
svideo_rtsp_set_drawcb(svideo_rtsp *h, draw_func cb, void *ud){
	h->draw_func = cb;
	h->draw_ud = ud;
}

int 
svideo_rtsp_get_fps(svideo_rtsp *h){
	return h->fps;
}



int svideo_init(void){
	if (hv_winapi_init()) return -1;
	avcodec_register_all();
	av_register_all();
	avformat_network_init();
	sdk_init = 1;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	return 0;
}




