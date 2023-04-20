/**
 * @File Name: timerCall.h
 * @brief  基于task任务机制框架之上，再次封装一个比toolkit更多功能，完美的框架。
 * @authorjieping.zhu (jieping.zhu@seres.cn)
 * @Version : 1.0
 * @Creat Date : 2023-04-19
 * 
 */
#ifndef _TIMER_CALL_H_
#define _TIMER_CALL_H_
#include <time.h>
#include "taskCall.h"
#include "dlist.h"
#include "common.h"

#define TIMER_LIFETIME_MAX  32  /* 定义最大支持多少种不同的超时时间 */
#define TIMER_ELEM_POSITION_UNKNOWN 0xFF

/* 定时器类型 */
typedef enum {
	TIMER_ONCE = 0,   /* 一次性定时任务 */
	TIMER_LOOP = 1    /* 循环定时任务 */
} TimerType_e;

/*定时器回调函数*/
typedef void (* TimerHandler_f) (void *);

/* 定时任务结点 双向队列*/
typedef struct TimerNode
{
	struct TimerNode *next;        /* add remove 时需要这俩打配合，O(1)时间复杂度去增加删除它*/
	struct TimerNode *prev;         
	TimerType_e  timerType;         /*见枚举 自定义了两种定时器 循环和单次*/
    MsgQueue_e   taskMq;            /* 该定时任务所属的队列(超时后指定由哪个线程执行) */
	uint32 whenTimeout;             /* 当前定时任务什么时候超时 刷新原理：当前时间戳+用户设定超时的客观时间段，例如3秒钟*/
	TimerHandler_f handler;         /* 超时回调函数 */
	void  *handlerArg;              /* 回调函数参数 ，不需要时传NULL即可*/
} TimerNode_s;

/*
 * 定时器链表
 * - 以定时器的超时时间来区分定时任务链表
 * - 超时时间相同的定时任务放到同一链表下面
 * - 最大支持32种不同的超时时间（自定义的）
 */
typedef struct
{
	TimerNode_s timerNodeHead;      /* 定时任务链表(表头) */
	uint32 lifetime;                /* 当前定时任务链表的超时时间(秒) */
	uint32 nodeNum;                 /* 链表上的定时任务统计 */
	pthread_mutex_t mutex;          /* 当前定时任务链表的互斥锁(定时任务结点入队或出队时要先拿锁) */
} TimerList_s;

#define TIMER_LIST_EMPTY(LIST) DLIST_EMPTY(&((LIST)->tnhead))


void    *ThreadTimer(void *arg);
void    TimerInit(void);
void    TimerDel(MsgQueue_e mq, uint32 lifetime, TimerType_e timerType, TimerHandler_f handler, void *arg);
int     TimerAdd(MsgQueue_e mq, uint32 lifetime, TimerType_e timerType, TimerHandler_f handler, void *arg);







#endif