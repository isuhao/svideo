#ifndef UTILS_H_
#define UTILS_H_

#define FALSE 0
#define TRUE 1

#define TRACE_ENABLE

#define ERR       0
#define INFO      1

char* GetLevelStr(int iLevel);

#define TRACE_LEVEL INFO

#ifdef TRACE_ENABLE
#define TRACE(level, fmt, arg...)\
{\
	if(level<=TRACE_LEVEL)\
		printf("[%s]:"fmt, GetLevelStr(level), ## arg);\
}
#endif

#endif /* UTILS_H_ */
