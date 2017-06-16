#include "svideo.h"
#include <windows.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
typedef struct player{
	char *url;
	unsigned long long hwnd;
	char *protocol;
	int id;
	HANDLE hprocess;
} player_t;

#define MAX_PLAYER 20

static player_t _players[MAX_PLAYER];
static bool _init;
static int _count;
static HANDLE _hthread;
static bool _close;

static void 
create_player(player_t *p){
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si,0,sizeof(si));
	memset(&pi,0,sizeof(pi));
	si.cb = sizeof(si);
	char cmdline[4000];
	sprintf(cmdline,"./svideo/svideop.exe %s %llu %s",p->url,p->hwnd,p->protocol);
	BOOL r = CreateProcess(NULL,cmdline,NULL,NULL,1,CREATE_NO_WINDOW,NULL,NULL,&si,&pi);
	p->hprocess = pi.hProcess;
}

static int 
work(void *p){
	int i;
	while(!_close){
		if(_count == 0) {Sleep(1000);}
		for(i = 0; i<_count;i++){
			player_t *py = &_players[i];
			if(py->url && py->hprocess == 0){
				create_player(py);
			}
			else{
				DWORD r = WaitForSingleObject(py->hprocess,1000);
				if(r == WAIT_OBJECT_0 || r == WAIT_FAILED){
					py->hprocess = 0;
				}
			}
		}
	}
	system("taskkill /F /IM svideop.exe");
	return 0;
}

void 
svideo_init(){
	if(_init) return;
	system("taskkill /F /IM svideop.exe");
	_init = true;
	memset(_players,0,sizeof(_players));
	_hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)work, NULL, 0, 0); 
}

int
svideo_play(const char *url,unsigned long long hwnd,const char *protocol){
	if(_count == MAX_PLAYER) return -1;
	player_t p = {strdup(url),hwnd,strdup(protocol),0,0};
	int i = _count++;
	_players[i] = p;
	return 0;
}

void
svideo_close_all(){
	_close = true;
	WaitForSingleObject(_hthread,INFINITE);
}

