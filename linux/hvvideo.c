#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

#include "hvplayer.h"

struct player_arg{
	char *addr;
	char hwnd[10];
	char w[5];
	char h[5];
	char protocol[5];
};
struct player{
	pid_t pid;
	bool has_create;
	bool has_null;
	struct player_arg parg;
};


#define MAX_PLAYER 10
static bool has_init=false;
static struct player players[MAX_PLAYER];
static bool has_listen=false;
static bool has_exit;
void
hvdevicevideo_init(){
	for(int i=1;i<MAX_PLAYER;i++){
		players[i].pid=0;
		players[i].has_create=false;
		players[i].has_null=true;
		memset(&players[i].parg,0,sizeof(struct player_arg));
	}
	has_init=true;
}


int
hvplayer_create(){
	if(!has_init){
		fprintf(stderr,"sdk no init,pelase use hvdevicevideo_init()\n");
		exit(0);
	}
	for(int i=1;i<MAX_PLAYER;i++){
		if(players[i].has_null){
			players[i].has_null=false;
			return i;
		}
	}
	return 0;
}

int
hvplayer_set_option(int id,int op,char *value){
	if(id<=0||id>=MAX_PLAYER) return -1;
	struct player *p=&players[id];
	switch(op){
		case HVPLAYER_OP_ADDR:
			if(p->parg.addr) 
				free(p->parg.addr);
			p->parg.addr=strdup(value);
			return 0;
		case HVPLAYER_OP_HWND:
			strcpy(p->parg.hwnd,value);
			return 0;
		case HVPLAYER_OP_WIDTH:
			strcpy(p->parg.w,value);
			return 0;
		case HVPLAYER_OP_HEIGHT:
			strcpy(p->parg.h,value);
			return 0;
		case HVPLAYER_OP_PROTOCOL:
			strcpy(p->parg.protocol,value);
			return 0;
	}
	return -1;
}


static inline void
collection_player(struct player *p){
	pid_t pid=waitpid(p->pid,NULL,WNOHANG);
	if(pid>0){
		p->pid=0;
		p->has_create=true;
	}
}

static inline void
new_player(struct player *p){
	pid_t pid=fork();
	if(pid==0){
		execl("hvplayer","hvplayer",p->parg.addr,p->parg.hwnd,p->parg.w,p->parg.h,p->parg.protocol,NULL);
	}
	else if(pid>0){
		p->pid=pid;
	}
}


static void *
player_listen(void *p){
	printf("begin listen\n");
	while(!has_exit){
		bool no_player=true;
		for(int i=1;i<MAX_PLAYER;i++){
			if(players[i].has_null){
				continue;
			}
			if(players[i].pid>0){
				collection_player(&players[i]);
				no_player=false;
			}
			if(players[i].pid==0&&players[i].has_create){
				printf("begin create player...\n");
				new_player(&players[i]);
				no_player=false;
			}
		}
		if(no_player) break;
		sleep(2);
	}
	system("pkill hvplayer");
	for(int i=1;i<MAX_PLAYER;i++){
		if(players[i].has_null)
			continue;
		if(players[i].pid>0){
			waitpid(players[i].pid,NULL,0);
			players[i].has_create=false;
		}
	}
	has_listen=false;
}


int
hvplayer_play(int id){
	if(id<=0||id>=MAX_PLAYER) return -1;
	players[id].has_create=true;
	if(!has_listen){
		pthread_t pt=0;
		has_exit=false;
		pthread_create(&pt,0,player_listen,NULL);
		has_listen=true;

	}
	return 0;
}

int 
hvplayer_close(int id){
	if(id<=0||id>=MAX_PLAYER) return -1;
	if(players[id].pid>0){
		players[id].has_null=true;
		kill(players[id].pid,SIGKILL);
		players[id].pid=0;
	}
	return 0;
}

void
hvplayer_closeall(){
	has_exit=true;
}


