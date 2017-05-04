#ifndef HVPLAYER_H
#define HVPLAYER_H
#include "hvtype.h"

struct svideo_rtsp;
typedef struct svideo_rtsp svideo_rtsp;
/**
 *播放退出回调
 *@param h 播放器指针
 *@param state 退出状态
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
 *初始化一个播放器
 *@param hwnd 一个表示画面显示区域的id,windows上为窗口句柄
 *@param url rtsp地址
 *@return NULL失败,>0为一个播放器指针
 */
HVEXP svideo_rtsp *svideo_rtsp_new(int32_t hwnd, const char *url);


/**
 *播放一个rtsp视频,该方法是一个非阻塞函数,内部会创建一个线程去执行播放
 *任务，自动无限重连，直到调用svideo_rtsp_close
 *
 *@param h 一个指向播放器的指针
 *@return 成功为0,失败-1
 */
 HVEXP int svideo_rtsp_play(svideo_rtsp *h);

/**
 *获取播放器的状态,成功会设置state
 *
 *@param h 一个指向播放器的指针
 *@return player_state枚举的值
 */
 HVEXP int svideo_rtsp_getstate(svideo_rtsp *h);



/**
 *停止播放,并结束播放线程,该方法会阻塞至播放线程结束,同时释放svideo_rtsp句柄
 *
 *@param h 一个指向播放器的指针
 */
 HVEXP void svideo_rtsp_close(svideo_rtsp *h);


 HVEXP int svideo_rtsp_get_fps(svideo_rtsp *h);

/**
 *注册播放结束回调,svideo_rtsp_close后回调会被调用
 *
 *@param h 一个指向播放器的指针
 *@param cb 具体见回调定义
 *@waring 多次注册会覆盖
 *@waring 不要阻塞该调用线程
 */
 HVEXP void svideo_rtsp_set_endcb(svideo_rtsp *h, playend_cb cb);
 /************************************************************************/
 
 
 HVEXP void svideo_rtsp_set_drawcb(svideo_rtsp *h,draw_func cb,void *ud);
 
 /*SDK初始化,必须在使用SDK之前初始化
  *@return 0成功
 /************************************************************************/
 HVEXP int svideo_init(void);




#endif
