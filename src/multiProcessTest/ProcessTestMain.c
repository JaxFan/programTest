/**
 * @File Name: ProcessTestMain.c
 * @brief  创造一个僵尸进程，并见证它是如何在进程中存在的。
 * 使用方式：
 * 1. 根据教程生成本可执行程序
 * 2.创建一个新的terminal : ps 查看僵尸进程。
 * ps ux | awk '{if($8=="Z+") print}'

 * @author: jieping.zhu (jax482@foxmail.com)
 * @Version : 1.0
 * @Creat Date : 2023-09-21
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include "ProcessTest.h"


#ifdef FUNCUN
int main (void)
{
    printf("hellos multiProcessTest age:%d\n",HELLO);

    pid_t pid,piduse,piduse2;
    printf("----------\n");
    pid = fork();
    if(pid == -1)
    {
        perror("fork error\n");
    }
    else if(pid > 0)//父进程
    {
        piduse = getpid();
        printf("fork father pid:%d\n",piduse);
        while(1)
        {

            printf(" father awake\n");
            sleep(2);
        }


        #if 1
        
        #endif
    }
    else if(pid == 0)
    {
        printf("fork son ,later exit\n");

    }


    return 0;
}
#endif
#define TEST_HOOK_FUN
#ifdef TEST_HOOK_FUN
#include <signal.h>
#include <stdlib.h>

/**
 * @brief  通过signal捕获ctrl+c软中断信号，并做一些事情。
 * http://t.csdn.cn/CsCD6
 * @param  sig: 本函数中永远都只能是：SIGINT
 */
void sigint_handler(int sig)
{
    if(sig == SIGINT)
    {
        printf("\nDo Something Before Ctr+C\n");
        sleep(3);
        exit(0);
    }
}
int main (int argc,char argv[])
{
    signal(SIGINT,sigint_handler);
    while(1)
    {
        printf("hellos hooking ctrl+C age:%d\n",HELLO);
        sleep(1);

    }
    printf("Do Over\n");
    return 0;
}

#endif