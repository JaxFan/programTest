/**
 * @File Name: timerCall.c
 * @brief  
 * 本次支持静态方式创建定时器。
 * 
 * TODO
 * 1.可以考虑动态，静态两种初始化方式。
 * @authorjieping.zhu (jax482@foxmail.com)
 * @Version : 1.0
 * @Creat Date : 2023-04-19
 * 
 */

#include "timerCall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/*
 * 定时器链表数据结构定义
 * 定时器最大支持32种不同的超时时间，每个数组元素代表一个超时时间，下挂一条双向链表
 * 该链表下的所有定时任务结点的超时时间一样。
 */
static TimerList_s TimerList[TIMER_LIFETIME_MAX];

/**
 * @brief  init: 1. listinit 2. mutex init 3. timerlist 内部成员初始化
 */
void TimerInit(void)
{
    TimerList_s *timerList = NULL;/*链表指针 用于指向静态定时器表 方便初始化*/
    int i = 0;
    for(i=0;i<TIMER_LIFETIME_MAX;i++)/*便利表中所有时间类型的定时器*/
    {
        /*1. listinit*/
        timerList = &TimerList[i];
        DLIST_INIT(&timerList->timerNodeHead);
        
        /*2. mutex init*/
        pthread_mutex_init(&timerList->mutex,NULL);

        /*inner init*/
        timerList->lifetime = 0;
        timerList->nodeNum  = 0;
        /*至此，timerlist成员全部初始化完成，不存在遗漏。*/
    }
}

/**
 * @brief  灵魂所在 ，完全可以写发明专利的级别！！！ 通过探索最短超时时间完成一次最优定时器遍历。将当前绝对时间下所有超时的定时器全部下链
 * @param  lifetime: 
 * @return int: 
 */
static int TimerGetPosition(uint32 lifetime)
{
	TimerList_s *timerList = NULL;
	int position = TIMER_ELEM_POSITION_UNKNOWN;
	int i = 0;
		
	for (i = 0; i < TIMER_LIFETIME_MAX; i++)
	{
		timerList = &TimerList[i];

		/* 找到相同超时时间的链表，不再继续往后遍历，返回当前位置 */
		if (timerList->lifetime == lifetime)
		{
			position = i;
			break;
		}

		/*
		 * 标记第一个没有定时任务链表的元素
		 * 如果在TimerList上没有找到与lifetime相同的数组元素，将使用position所在的数组元素
		 * 需要遍历完整个数组，以确保没找到相同超时时间的链表
		 */
		if ((0 == timerList->nodeNum) && (TIMER_ELEM_POSITION_UNKNOWN == position))
		{
			timerList->lifetime = lifetime;
			position = i;
		}
	}
	
	return position;
}

/**
 * @brief  
 * @param  mq: 
 * @param  lifetime: 
 * @param  timerType: 
 * @param  handler: 
 * @param  arg: 
 * @return int: 
 */
static int TimerIsExist(MsgQueue_e mq, uint32 lifetime, TimerType_e timerType, TimerHandler_f handler, void *arg)
{
	TimerList_s *timerList = NULL;
	TimerNode_s *timerNode = NULL;
	TimerNode_s *timerNodeNext = NULL;
	int i = 0;
	int ret = ERR;
	
	for (i = 0; i < TIMER_LIFETIME_MAX; i++)
	{
		timerList = &TimerList[i];

		pthread_mutex_lock(&timerList->mutex);
		if (timerList->lifetime == lifetime)  /* 找到对应的超时链表 */
		{
			if (timerList->nodeNum > 0)
			{
				timerNode = timerList->timerNodeHead.next;
				
				while(timerNode != &timerList->timerNodeHead)
				{
					timerNodeNext = timerNode->next;

					if ((timerNode->timerType == timerType)
                        && (timerNode->taskMq == mq)
						&& (timerNode->handler == handler)
						&& (timerNode->handlerArg == arg))
					{
						ret = OK;
						break;
					}
					
					timerNode = timerNodeNext;
				}
			}
			pthread_mutex_unlock(&timerList->mutex);
			break;
		}
		pthread_mutex_unlock(&timerList->mutex);
	}
	
	return ret;
}

/**
 * @brief  
 * @param  mq: 
 * @param  lifetime: 
 * @param  timerType: 
 * @param  handler: 
 * @param  arg: 
 * @return int: 
 */
int TimerAdd(MsgQueue_e mq, uint32 lifetime, TimerType_e timerType, TimerHandler_f handler, void *arg)
{

	TimerList_s *timerList = NULL; //定时器链表
	TimerNode_s *timerNode = NULL; //定时结点
	int position = TIMER_ELEM_POSITION_UNKNOWN;
	time_t curTime = time(NULL);/*获取当前系统时间戳 作为下链的依据，采用绝对时间，此处是否有隐患？*/

	/* 如果是循环定时器，判断定时器是否已存在 */
	if (TIMER_LOOP == timerType)
	{
		if (OK == TimerIsExist(mq, lifetime, TIMER_LOOP, handler, arg))
		{
			return OK;
		}
	}
    /* 根据超时时间获取定时器链表应当插入的位置 */
	position = TimerGetPosition(lifetime);

	if (TIMER_ELEM_POSITION_UNKNOWN == position)
	{
		/* 定时器数组已用完，没有找到合适的位置加入 */
		printf("timer lifetime is reached the max number(%d)", TIMER_LIFETIME_MAX);
		return ERR;
	}    

	/* 新建一个定时结点 */
	MALLOC(timerNode, sizeof(TimerNode_s));
	if (NULL == timerNode)
	{
		return ERR;
	} 

	DLIST_INIT(timerNode);
	timerNode->timerType = timerType;
    timerNode->taskMq = mq;
	timerNode->handler = handler;
	timerNode->handlerArg = arg;
	timerNode->whenTimeout = curTime + lifetime;

	/* 定时结点上链 */
	timerList = &TimerList[position];
	pthread_mutex_lock(&timerList->mutex);
	DLIST_ADD_TAIL(&timerList->timerNodeHead, timerNode);
	timerList->nodeNum++;
	pthread_mutex_unlock(&timerList->mutex);

	return OK;       
}

void TimerDel(MsgQueue_e mq, uint32 lifetime, TimerType_e timerType, TimerHandler_f handler, void *arg)
{
	TimerList_s *timerList = NULL;
	TimerNode_s *timerNode = NULL;
	TimerNode_s *timerNodeNext = NULL;
	int i = 0;
	
	for (i = 0; i < TIMER_LIFETIME_MAX; i++)
	{
		timerList = &TimerList[i];

		pthread_mutex_lock(&timerList->mutex);
		if (timerList->lifetime == lifetime)  /* 找到对应的超时链表 */
		{
			if (timerList->nodeNum > 0)
			{
				timerNode = timerList->timerNodeHead.next;
				
				while(timerNode != &timerList->timerNodeHead)
				{
					timerNodeNext = timerNode->next;
                    /* 必须确保所有入参一致，才能删除定时器！！*/
					if ((timerNode->timerType == timerType)
                        && (timerNode->taskMq == mq)
						&& (timerNode->handler == handler)
						&& (timerNode->handlerArg == arg))
					{
						DLIST_REMOVE_NODE(timerNode);
						FREE(timerNode);
						timerList->nodeNum--;
					}
					
					timerNode = timerNodeNext;
				}
			}
			pthread_mutex_unlock(&timerList->mutex);
			break;
		}
		pthread_mutex_unlock(&timerList->mutex);
	}
	
	return;
}


void *ThreadTimer(void *arg)
{
	TimerList_s *timerList = NULL;
	TimerNode_s *timerNode = NULL;
	TimerNode_s *timerNodeNext = NULL;
	TaskNode_s   taskNode = {0};
	int i = 0;
	time_t curTime = 0;

	printf("running  timer");
	
	while(1)
	{
		curTime = time(NULL);
		
		for (i = 0; i < TIMER_LIFETIME_MAX; i++)
		{
			timerList = &TimerList[i];

			pthread_mutex_lock(&timerList->mutex);
			if (timerList->nodeNum > 0)
			{
				timerNode = timerList->timerNodeHead.next;
				
				while(timerNode != &timerList->timerNodeHead)
				{
					timerNodeNext = timerNode->next;

					/*
					 * 定时任务结点按超时时间顺序上链的
					 * 如果当前结果未超时，其往后的结点都没有超时
					 */
					if (timerNode->whenTimeout > curTime)
					{
						break;
					}

					/* 当前任务超时，定时结点下链 */
					DLIST_REMOVE_NODE(timerNode);

					/* 加入处理队列 */
					memset(&taskNode, 0, sizeof(taskNode));
					taskNode.taskType = TASK_LEARN_MATH;
					taskNode.data    = timerNode->handler;
					taskNode.taskArg  = timerNode->handlerArg;
					if (ERR == MsgEnQueue(timerNode->taskMq, &taskNode))/* MsgEnQueue：*/
					{
						printf("-timer message EnQueue error");
					}

					if (TIMER_LOOP == timerNode->timerType)
					{
						/* 循环定时任务，更新超时时间，重新上链 */
						timerNode->whenTimeout = curTime + timerList->lifetime;
						DLIST_ADD_TAIL(&timerList->timerNodeHead, timerNode);
					}
					else
					{
						/* 一次性任务，释放结点 */
						FREE(timerNode);
						timerList->nodeNum--;
					}

					timerNode = timerNodeNext;
				}
			}
			pthread_mutex_unlock(&timerList->mutex);
		}
		sleep(1);
	}
	
	return NULL;    
}







