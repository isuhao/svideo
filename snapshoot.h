#ifndef SNAPSHOOT_H
#define SNAPSHOOT_H
#include "hvtype.h"

struct snapshoot_task
{
	hvframe *frame;
	char *path;
	void *ud;
};

typedef struct snapshoot_task snapshoot_task_t;
typedef void (*complete_cb)(snapshoot_task_t *task);

int snapshoot_supper_format(const char *path);
void snapshoot_task(snapshoot_task_t *task,complete_cb cb);





#endif