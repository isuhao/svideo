#include "snapshoot.h"
#include "hvpfconvert.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


struct snap_task{
	complete_cb cb;
	snapshoot_task_t *task;
};

static int32_t format[]={
	0x00706d62 //bmp
};

#define format_n() (sizeof(format)/sizeof(int32_t))

int 
snapshoot_supper_format(const char *path)
{
    int n=strlen(path);
	int32_t code=*(int32_t*)(path+(n-4));
	bool find=false;
	for(int i=0;i<format_n();i++){
		if(format[i]==code){
			find=true;
			break;
		}
	}
	return find;
}

static int 
work(void *p)
{
	
	struct snap_task *task=p;
	snapshoot_task_t *t=task->task;
	int rgb_n=t->frame->w*t->frame->h*3;
	hvpfconvert *convert=hvpfconvert_new(t->frame->w, t->frame->h, t->frame->w, t->frame->h, PF_YUV420P, PF_BGR24);
	char *rgb=malloc(rgb_n);
	hvpfconvert_convert2(convert, t->frame, rgb);
	hvpfconvert_free(&convert);
	
	BITMAPFILEHEADER head;
	head.bfType=0x4d42;
	head.bfSize=sizeof(head)+sizeof(BITMAPINFOHEADER)+rgb_n;
	head.bfReserved1=0;
	head.bfReserved2=0;
	head.bfOffBits=sizeof(head)+sizeof(BITMAPINFOHEADER);
	BITMAPINFOHEADER info;
	info.biBitCount = 24;
	info.biClrImportant = BI_RGB;
	info.biClrUsed = 0;
	info.biCompression = 0;
	info.biHeight = -t->frame->h;
	info.biWidth = t->frame->w;
	info.biPlanes = 1;
	info.biSize = sizeof(info);
	info.biSizeImage = rgb_n;
	info.biXPelsPerMeter = 0;
	info.biYPelsPerMeter = 0;
	FILE *f=fopen(t->path,"wb");
	fwrite(&head,1,sizeof(head),f);
	fwrite(&info,1,sizeof(info),f);
	fwrite(rgb,1,rgb_n,f);
	fclose(f);
	free(rgb);
	//MessageBox(NULL,"snap frame",NULL,MB_OK);
	task->cb(t);
	free(task);
}

void snapshoot_task(snapshoot_task_t *task,
					complete_cb cb){
	struct snap_task *t=malloc(sizeof(*t));
	t->cb=cb;
	t->task=task;
	HANDLE h=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)work,(LPVOID)t,0,0);
	CloseHandle(h);
}