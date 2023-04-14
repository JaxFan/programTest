/**
 * @File Name: main.c
 * @brief  用于测试一些框架
 * @authorjieping.zhu (jieping.zhu@seres.cn)
 * @Version : 1.0
 * @Creat Date : 2023-04-14
 * 
 */
#include <stdio.h>
#include "taskCall.h"
#include <unistd.h>/*sleep()*/
#include <string.h>
#include <stdlib.h>
#define PALYLOAD_LEN 24
char g_mainPlan[][PALYLOAD_LEN]=
{
    {"now is 06:00"},
    {"It's sunny"},
    {"you should go"}
};

/*生产者 */
void *CreateMain(void *arg)
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
            printf("CreateMain play The %d msg: No.%d :%s\n",count++,i,g_mainPlan[i]);  
        }
        sleep(1);
    }
    while(1);
}

void *CreatePlay(void *arg)
{
    //printf("CreateLearn learn action\n");

}

void *CreateLearn(void *arg)
{
    //printf("CreateLearn learn action\n");

}

void ProcMainTime(void *msg, int msgLen)
{
    if(msg == NULL)
    {
        exit (1);       
    }
    printf("get ProcMainTime val is %s\n",(char*)msg);

}
void ProcMainWeather(void *msg, int msgLen)
{
    if(msg == NULL)
    {
        exit (1);       
    }
    printf("get ProcMainWeather val is %s\n",(char*)msg);
}
void ProcMainPlan(void *msg, int msgLen)
{
    if(msg == NULL)
    {
        exit (1);       
    }
    printf("get ProcMainPlan val is %s\n",(char*)msg);
}

static void ThreadMain(void)
{
    TaskNode_s taskNode ={0};
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
            default:
                break;
            }
        }
    }
}

int main(void)
{
    MsgQueueInit();
    printf("from cmake project\n");

    pthread_t tidPlay,tidLearn,tidMain;

    pthread_create(&tidMain,0, CreateMain,NULL);
    pthread_create(&tidPlay,0, CreatePlay,NULL);
    pthread_create(&tidLearn,0,CreateLearn,NULL);
    pthread_detach(tidPlay);
    pthread_detach(tidMain);
    pthread_detach(tidLearn);
    ThreadMain();




    return 0;
}