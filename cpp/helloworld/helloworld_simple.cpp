#include <stdio.h>

#define DEBUG
#define LOG_TAG "main"

#define LOGE(fmt, args...) printf("[%s]:"fmt, LOG_TAG, ##args)
#ifdef DEBUG
#define LOGI(fmt, args...) printf("[%s]:"fmt, LOG_TAG, ##args)
#else
#define LOGI(fmt, args...) 
#endif

#include "util.h"

int main(int argc, char* argv[])
{
    float start = cur_ms();

    LOGI(">>main\n");

    LOGI("HelloWorld\n");

    float end = cur_ms();

    LOGI("runt time:%.3f\n", (end-start));

    LOGI("<<main\n");
    return 0;
}
