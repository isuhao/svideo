#ifndef HVPLAYER_H
#define HVPLAYER_H
#include "hvtype.h"

struct svideo_rtsp;
typedef struct svideo_rtsp svideo_rtsp;
/**
 *�����˳��ص�
 *@param h ������ָ��
 *@param state �˳�״̬
 */
typedef void (*playend_cb)(svideo_rtsp *h);

typedef void (*draw_func)(struct hvframe *f,void *ud);
enum player_state
{
	PLAYER_CONNECTING=0,
	PLAYER_PLAYING,
	PLAYER_OFF
};



/**
 *��ʼ��һ��������
 *@param hwnd һ����ʾ������ʾ�����id,windows��Ϊ���ھ��
 *@param url rtsp��ַ
 *@return NULLʧ��,>0Ϊһ��������ָ��
 */
HVEXP svideo_rtsp *svideo_rtsp_new(int32_t hwnd, const char *url);


/**
 *����һ��rtsp��Ƶ,�÷�����һ������������,�ڲ��ᴴ��һ���߳�ȥִ�в���
 *�����Զ�����������ֱ������svideo_rtsp_close
 *
 *@param h һ��ָ�򲥷�����ָ��
 *@return �ɹ�Ϊ0,ʧ��-1
 */
 HVEXP int svideo_rtsp_play(svideo_rtsp *h);

/**
 *��ȡ��������״̬,�ɹ�������state
 *
 *@param h һ��ָ�򲥷�����ָ��
 *@return player_stateö�ٵ�ֵ
 */
 HVEXP int svideo_rtsp_getstate(svideo_rtsp *h);



/**
 *ֹͣ����,�����������߳�,�÷����������������߳̽���,ͬʱ�ͷ�svideo_rtsp���
 *
 *@param h һ��ָ�򲥷�����ָ��
 */
 HVEXP void svideo_rtsp_close(svideo_rtsp *h);


 HVEXP int svideo_rtsp_get_fps(svideo_rtsp *h);

/**
 *ע�Ქ�Ž����ص�,svideo_rtsp_close��ص��ᱻ����
 *
 *@param h һ��ָ�򲥷�����ָ��
 *@param cb ������ص�����
 *@waring ���ע��Ḳ��
 *@waring ��Ҫ�����õ����߳�
 */
 HVEXP void svideo_rtsp_set_endcb(svideo_rtsp *h, playend_cb cb);
 /************************************************************************/
 
 
 HVEXP void svideo_rtsp_set_drawcb(svideo_rtsp *h,draw_func cb,void *ud);
 
 /*SDK��ʼ��,������ʹ��SDK֮ǰ��ʼ��
  *@return 0�ɹ�
 /************************************************************************/
 HVEXP int svideo_init(void);




#endif
