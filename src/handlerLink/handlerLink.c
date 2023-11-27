/**
 * @File Name: handlerLink.c
 * @brief 责任链 C语言版本 演示状态机如何简单运转的。
 * 
 * 
 * @author: jieping.zhu (jax482@foxmail.com)
 * @Version : 1.0
 * @Creat Date : 2023-11-27
 * 
 */

#include <stdio.h>
#include <stdlib.h>

// 定义一个处理器接口
typedef struct Handler {
    void (*handle)(struct Handler* self, int request);
    struct Handler* next;
} Handler;


// 处理器1
void RmtPreCheck(Handler* self, int request) {
    if (request <= 10) {
        printf("Handler 1 handles the request: %d\n", request);
    } else if (self->next != NULL) {
        self->next->handle(self->next, request);
    } else {
        printf("Request cannot be handled\n");
    }
}

Handler* RmtCreatePreCheckHandler() {
    Handler* handler = (Handler*)malloc(sizeof(Handler));
    handler->handle = RmtPreCheck;
    handler->next = NULL;
    return handler;
}

// 处理器2
void RmtExecDoing(Handler* self, int request) {
    if (request <= 20) {
        printf("Handler 2 handles the request: %d\n", request);
    } else if (self->next != NULL) {
        self->next->handle(self->next, request);
    } else {
        printf("Request cannot be handled\n");
    }
}

Handler* RmtCreateExecDoingHandler() {
    Handler* handler = (Handler*)malloc(sizeof(Handler));
    handler->handle = RmtExecDoing;
    handler->next = NULL;
    return handler;
}

int main() {

    // 创建预检查的责任链
    Handler* preCheckHandle = RmtCreatePreCheckHandler();

    // 创建执行责任链
    Handler* execDoingHandle = RmtCreateExecDoingHandler();

    // 指定预检查的责任链下一责任链
    preCheckHandle->next = execDoingHandle;


    // 测试环节

    // 情况1
    preCheckHandle->handle(preCheckHandle, 5);
    // 情况2
    preCheckHandle->handle(preCheckHandle, 50);

    // 资源回收
    free(preCheckHandle);
    free(execDoingHandle);

    return 0;
}










