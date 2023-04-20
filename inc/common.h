#ifndef _COMMON_TYPE_H_
#define _COMMON_TYPE_H_

#define MALLOC(P, SIZE) \
	do { \
		P = (typeof(*(P))*)malloc(SIZE);\
		if (NULL != P) { \
			memset(P, 0, SIZE); \
		} \
	}while(0)
    
#define FREE(P) \
	do { \
		if (NULL != P) { \
			free(P); \
			P = NULL; \
		} \
	}while(0)

/* 返回值定义 */
typedef enum
{
	OK = 0,
	ERR,
	PARAM_ERR,
	RET_MAX
} Ret_e;


#endif