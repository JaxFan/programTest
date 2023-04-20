/**
 * @File Name: main.c
 * @brief  用于测试一些框架
 * @authorjieping.zhu (jax482@foxmail.com)
 * @Version : 1.0
 * @Creat Date : 2023-04-14
 * 
 */
#include <stdio.h>
#include "taskCall.h"
#include "timerCall.h"
#include <unistd.h>/*sleep()*/
#include <string.h>
#include <stdlib.h>

/*用于显示每个任务到期之后，要打印的文字的长度上限*/
#define PALYLOAD_LEN 24

/*以下几个都是测试语句，实际使用时可以任意切换成自己的数据或者指针。*/
char g_mainPlan[][PALYLOAD_LEN]=
{
    {"now is 06:00"},
    {"It's sunny"},
    {"you should go"}
};

char g_mainLearn[][PALYLOAD_LEN]=
{
    {"learn math"},
    {"learn English"},
    {"learn code"}
};

#define CREATE_EVENT_TIME 5/*任务调度周期*/
#define CREATE_TIMER_TIME 5/*定时器超时下链时间*/

/*生产者 */
static void *CreateMain(void *arg)
{
    TaskNode_s taskNode ={0};
    int ret=0;
    static int count=0;
    do
    { 
        for(int i=0;i<3;i++)
        {
            taskNode.data = (void *)g_mainPlan[i];
            taskNode.taskType = TASK_MAIN_TIME + i;
            ret = MsgEnQueue(MQ_MAIN,&taskNode);
            if(TASK_MQ_ERR == ret)
            {
                LOG_ERR("TASK_MQ_ERR en queue err ret %d err:%d",ret,TASK_MQ_ERR);
            }
                      
            taskNode.dataLen = PALYLOAD_LEN;
        }
        sleep(CREATE_EVENT_TIME);
    }
    while(1);
}

static void *CreatePlay(void *arg)
{
    //printf("CreateLearn learn action\n");

}

static void *CreateLearn(void *arg)
{
    //printf("CreateLearn learn action\n");

}

static void ProcMainTime(void *msg, int msgLen)
{
    if(msg == NULL)
    {
        exit (1);       
    }
    printf("get ProcMainTime val is %s\n",(char*)msg);

}
static void ProcMainWeather(void *msg, int msgLen)
{
    if(msg == NULL)
    {
        exit (1);       
    }
    printf("get ProcMainWeather val is %s\n",(char*)msg);
}
static void ProcMainPlan(void *msg, int msgLen)
{
    if(msg == NULL)
    {
        exit (1);       
    }
    printf("get ProcMainPlan val is %s\n",(char*)msg);
}

static void TimerCallback_Learn_Math(void *arg)
{
    printf("\n\n");
    printf("get TimerCallback_Learn_Math val is:     ----%s----\n",(char*)arg);
    printf("\n\n");
    //TimerDel(MQ_MAIN,CREATE_TIMER_TIME,TIMER_LOOP,TimerCallback_Learn_Math,NULL);
    //printf("TimerAdd del quit it\n");
}

static void ThreadMain(void)
{
    TaskNode_s taskNode ={0};
    TimerHandler_f timerHandler = NULL;
    
    int ret = TimerAdd(MQ_MAIN,CREATE_TIMER_TIME,TIMER_LOOP,TimerCallback_Learn_Math,(void *)g_mainLearn[0]);
    if(ret != OK)
    {
            printf("TimerAdd err\n");

    }
    while(1)
    {
        MQ_SEM_WAIT(MQ_MAIN);/*等待生产者增加一个主线任务*/
        if(TASK_MQ_OK == MsgDeQueue(MQ_MAIN,&taskNode));/* 从MQ_MAIN中pop任务出来，并用 taskNode接收*/
        {
            switch (taskNode.taskType)
            {
            case TASK_MAIN_TIME:
                    ProcMainTime(taskNode.data,taskNode.dataLen);
                break;
            case TASK_MAIN_WEATHER:
                    ProcMainWeather(taskNode.data,taskNode.dataLen);
                break;
            case TASK_MAIN_PLAN:
                    ProcMainPlan(taskNode.data,taskNode.dataLen);
                break; 
            case TASK_LEARN_MATH:
					timerHandler = (TimerHandler_f)taskNode.data;
					if (NULL != timerHandler)
					{
						timerHandler(taskNode.taskArg);
					}
                break;
            default:
                break;
            }
        }
    }
}

int main(void)
{
    MsgQueueInit();
    TimerInit();
    printf("Sys Start\n");

    pthread_t tidPlay,tidLearn,tidMain;

    pthread_create(&tidMain,0, CreateMain,NULL);/* 主线程负责作为生产者 循环产生计划的三种事，交由ThreadMain处理*/
    pthread_create(&tidPlay,0, CreatePlay,NULL);/*未使用 */
    pthread_create(&tidLearn,0,ThreadTimer,NULL);/* 学习类任务放在定时器中执行，超时之后将回调函数push到执行队列中，交由ThreadMain处理*/

    pthread_detach(tidPlay);
    pthread_detach(tidMain);
    pthread_detach(tidLearn);

    ThreadMain();/*1. 定时器在此处启动；2.循环处理MQ_MAIN队列中待执行的任务。*/
    printf("Sys Over\n");



    return 0;
}