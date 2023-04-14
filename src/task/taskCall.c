/**
 * @File Name: taskCall.c
 * @brief  
 * @authorjieping.zhu (jieping.zhu@seres.cn)
 * @Version : 1.0
 * @Creat Date : 2023-04-14
 * 
 */
#include <stdlib.h>
#include "taskCall.h"

MsgQueue_s g_MsgQueue[MQ_NUM];/*创建一个消息队列实体数组 成立三个事件的任务队列 例如：play里可以有:csol lol 王者荣耀等玩耍任务 learn 里可以有 语文 高数 诗词*/




/**
 * @brief  
 * 初始化消息队列
 */
void MsgQueueInit(void)
{
    MsgQueue_s *pMq =NULL;
    int i = 0;

    /* 初始化三个类型的任务队列 ：指定队列大小需要满足2^N */
    g_MsgQueue[MQ_MAIN].queueSize = QUEUE_MAXNUM;
    g_MsgQueue[MQ_PLAY].queueSize = QUEUE_MAXNUM;
    g_MsgQueue[MQ_LEARN].queueSize = QUEUE_MAXNUM;

    for(i=0;i<MQ_NUM;i++)
    {
        pMq = &g_MsgQueue[i];
        pMq->queueSizeMask = pMq->queueSize-1;
        pMq->queue = (TaskNode_s*)malloc(sizeof(TaskNode_s)*pMq->queueSize);/*空间大小：单个节点大小× queueSize*/
        if (NULL == pMq->queue)
        {
        	exit(1);
        }
        pMq->front = 0;
        pMq->rear = 0;
        sem_init(&pMq->msgSem, 0, 0);
        pthread_mutex_init(&pMq->mutex, NULL);
    }
    return; 
}

/**
 * @brief  加减互斥锁，中间执行
 * @param  mq: 
 * @param  node: 
 * @return int: 
 */
int MsgEnQueue(MsgQueue_e mq, TaskNode_s *node)
{

    MsgQueue_s *pMq =  &g_MsgQueue[mq];
    uint16 rear = 0;
    uint16 next = 0;

    pthread_mutex_lock(&pMq->mutex);
    
    next = (pMq->rear + 1) & pMq->queueSizeMask;
    if (next == pMq->front)
    {
        LOG_INFO("msgQ %d full, front %d, rear %d, size %u, smask %u, taskType %d, dataLen %d",
            mq, pMq->front, pMq->rear, pMq->queueSize, pMq->queueSizeMask, node->taskType, node->dataLen);
        pthread_mutex_unlock(&pMq->mutex);
        return TASK_MQ_ERR;
    }

    rear = pMq->rear;
    
    pMq->queue[rear].taskType = node->taskType;
    pMq->queue[rear].param1  = node->param1;
    pMq->queue[rear].param2  = node->param2;
    pMq->queue[rear].param3  = node->param3;
    pMq->queue[rear].dataLen = node->dataLen;
    pMq->queue[rear].data    = node->data;
    pMq->queue[rear].taskArg = node->taskArg;
    
    pMq->rear = next;
    
    pthread_mutex_unlock(&pMq->mutex);

    sem_post(&pMq->msgSem);
    
	return TASK_MQ_OK;
}

int MsgDeQueue(MsgQueue_e mq, TaskNode_s *node)
{
    MsgQueue_s *pMq =  &g_MsgQueue[mq];
    uint16 front = 0;
    
    pthread_mutex_lock(&pMq->mutex);
    
    if (MQ_EMPTY(pMq))
    {
        LOG_INFO("msgQ empty, front %d, rear %d", pMq->front, pMq->rear);
        pthread_mutex_unlock(&pMq->mutex);
        return TASK_MQ_ERR;
    }

    front = pMq->front;
    
    node->taskType = pMq->queue[front].taskType;
    node->param1  = pMq->queue[front].param1;
    node->param2  = pMq->queue[front].param2;
    node->param3  = pMq->queue[front].param3;
    node->dataLen = pMq->queue[front].dataLen;
    node->data    = pMq->queue[front].data;
    node->taskArg = pMq->queue[front].taskArg;
    
    pMq->front = (front + 1) & pMq->queueSizeMask;

    pthread_mutex_unlock(&pMq->mutex);

    return TASK_MQ_OK;
}
