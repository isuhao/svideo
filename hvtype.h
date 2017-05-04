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

//yuv420p���ݽṹ
typedef struct hvframe
{
	int32_t h;            /**ͼ��߶�*/
	int32_t w;
	uint8_t *y;          /*y��������*/
	uint8_t *u;         /*u��������*/
	uint8_t *v;        /*v��������*/
	int32_t ypitch;   /*y����ÿ�����ݴ�С*/
	int32_t uvpitch; /*uv����ÿ�����ݴ�С*/
}hvframe;

#endif
