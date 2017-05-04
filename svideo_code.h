#ifndef HVCODE_H
#define HVCODE_H
#include "hvtype.h"

/*********************************************************************
*@describe 视频流解码器,对ffmpeg的浅封装
*@auth 易辉龙
*@date 2015-09-06
*@remark 只支持h264->yuv420p
**********************************************************************/

struct svideo_code;
typedef struct svideo_code svideo_code;
/**************************************************
*@describe 初始化解码器
*@reutrn 失败返回0
**************************************************/
 svideo_code * svideo_code_new(int w,int h);
/*************************************************
*@decribe 解码一帧数据
*@return 成功:0 失败:-1
*************************************************/
 int  svideo_code_decode(svideo_code *h, uint8_t * src, uint32_t src_size, uint8_t *dest, uint32_t dest_size);
//@decribe 释放解码器
 void  svideo_code_free(svideo_code **h);
#endif
