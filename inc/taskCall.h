/**
 * @File Name: taskCall.h
 * @brief 任务队列的框架设计 
 * @authorjieping.zhu (jieping.zhu@seres.cn)
 * @Version : 1.0
 * @Creat Date : 2023-04-14
 * 
 */

#ifndef _TASK_CALL_H_
#define _TASK_CALL_H_

#define QUEUE_MAXNUM 0x100 /* 每个消息队列能装多少个任务 2^N 可以分别赋值*/ 

#include <pthread.h>
#include <semaphore.h>
#include "datatype.h"
#include "log.h"

/* 任务类型 */
typedef enum
{

    TASK_PLAY_LOL = 1,      /* 玩耍任务 */
    TASK_PLAY_CSGO = 2,      /* 玩耍任务 */
    TASK_PLAY_QQ = 3,      /* 玩耍任务 */

    TASK_LEARN_MATH = 4,         /* 学习任务 */
    TASK_LEARN_ENGLISH = 5,         /* 学习任务 */
    TASK_LEARN_CODE = 6,         /* 学习任务 */

    TASK_MAIN_TIME = 7,         /* 学习任务 提示当前时间*/
    TASK_MAIN_WEATHER = 8,         /* 学习任务 其实当前天气*/
    TASK_MAIN_PLAN = 9,         /* 学习任务 其实该干嘛了*/
    TASK_NUM
} TaskType_e;

/* 消息队列枚举 用于入队传参 防止魔幻数字*/
typedef enum
{
    MQ_MAIN     = 0,    /* 主线程队列 */
    MQ_PLAY     = 1,    /* 远控线程队列 */
    MQ_LEARN    = 2,    /* 远控线程队列 */
    MQ_NUM
} MsgQueue_e;

/* 任务结点 */
typedef struct
{
    TaskType_e taskType;   /* 任务类型 */
    uint16 param1;            /* 任务参数1 */
    uint16 param2;            /* 任务参数2 */
    uint16 param3;            /* 任务参数3 */
    uint16 dataLen;           /* 数据长度 */
    void  *data;              /* 数据 */
    void  *taskArg;           /* 任务附加参数 */
} TaskNode_s;

/* 消息队列结构定义 双向循环队列*/
typedef struct
{
    TaskNode_s *queue;  /*指向队列的任务指针*/
    uint16 queueSize;   /*队列大小*/
    uint16 queueSizeMask;/*循环队列识别头尾用；方法： next = (pMq->rear + 1) & pMq->queueSizeMask;*/
    uint16 front;
    uint16 rear;
    sem_t msgSem;
    pthread_mutex_t mutex;
} MsgQueue_s;

/* 消息队列 出队入队结果返回 用于入队回传 防止魔幻数字*/
typedef enum
{
	TASK_MQ_OK = 0,
	TASK_MQ_ERR = 1
} TaskMqRet_e;


extern MsgQueue_s g_MsgQueue[];
#define MQ_EMPTY(Q)      ((Q)->front == (Q)->rear)
#define MQ_SEM_WAIT(MQ)  sem_wait(&g_MsgQueue[MQ].msgSem)

void MsgQueueInit(void);
int MsgEnQueue(MsgQueue_e mq, TaskNode_s *node);
int MsgDeQueue(MsgQueue_e mq, TaskNode_s *node);


#endif