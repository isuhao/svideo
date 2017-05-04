#ifndef HVCODE_H
#define HVCODE_H
#include "hvtype.h"

/*********************************************************************
*@describe ��Ƶ��������,��ffmpeg��ǳ��װ
*@auth �׻���
*@date 2015-09-06
*@remark ֻ֧��h264->yuv420p
**********************************************************************/

struct svideo_code;
typedef struct svideo_code svideo_code;
/**************************************************
*@describe ��ʼ��������
*@reutrn ʧ�ܷ���0
**************************************************/
 svideo_code * svideo_code_new(int w,int h);
/*************************************************
*@decribe ����һ֡����
*@return �ɹ�:0 ʧ��:-1
*************************************************/
 int  svideo_code_decode(svideo_code *h, uint8_t * src, uint32_t src_size, uint8_t *dest, uint32_t dest_size);
//@decribe �ͷŽ�����
 void  svideo_code_free(svideo_code **h);
#endif
