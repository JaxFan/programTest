
#ifndef _LOG_H_
#define _LOG_H_
#include <stdio.h>

#define LOG_ERR(FORMAT, ARG...)   printf("{%s}"FORMAT"\n", __FUNCTION__, ##ARG)
#define LOG_WARN(FORMAT, ARG...)  printf("{%s}"FORMAT"\n", __FUNCTION__, ##ARG)
#define LOG_INFO(FORMAT, ARG...)  printf("{%s}"FORMAT"\n", __FUNCTION__, ##ARG)
#define LOG_DEBUG(FORMAT, ARG...) printf("{%s}"FORMAT"\n", __FUNCTION__, ##ARG)


#endif