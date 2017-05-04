#ifndef HVTYPE_H
#define  HVTYPE_H
#include <stdint.h>

#define inline __inline
#define HVEXP __declspec(dllexport)








//typedef unsigned char uint8;
//typedef char int8;
//typedef signed short int int16;
//typedef unsigned short int uint16;
//typedef signed int int32;
//typedef unsigned int uint32;
//typedef long long int64;
//typedef unsigned long long uint64;

//yuv420p数据结构
typedef struct hvframe
{
	int32_t h;            /**图像高度*/
	int32_t w;
	uint8_t *y;          /*y分量数据*/
	uint8_t *u;         /*u分量数据*/
	uint8_t *v;        /*v分量数据*/
	int32_t ypitch;   /*y分量每行数据大小*/
	int32_t uvpitch; /*uv分量每行数据大小*/
}hvframe;

#endif
