


#ifndef HVPFCONVERT_H
#define HVPFCONVERT_H
#include "hvtype.h"
struct svideo_convert;
typedef struct svideo_convert svideo_convert;

enum pic_formart
{
	PF_RGB24=0,
	PF_BGR24,
	PF_YUV420P
};

#ifdef __cplusplus
extern "C"{
#endif

 HVEXP svideo_convert *svideo_convert_new(int sw, int sh, int dw, int dh, enum pic_formart srctype, enum pic_formart desttype);
 HVEXP void svideo_convert_free(svideo_convert **h);
 HVEXP int svideo_convert_convert(svideo_convert *h, uint8_t *src, uint8_t *dest);
 HVEXP int svideo_convert_convert2(svideo_convert *h, hvframe *src, uint8_t *dest);
 HVEXP uint32_t svideo_convert_get_size(int w, int h, enum pic_formart pt);

#ifdef __cplusplus
}
#endif

#endif
