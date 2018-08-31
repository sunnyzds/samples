#include <stdio.h>

#define DEBUG
#define LOG_TAG "main"

#define LOGE(fmt, args...) printf("[%s]:"fmt, LOG_TAG, ##args)
#ifdef DEBUG
#define LOGI(fmt, args...) printf("[%s]:"fmt, LOG_TAG, ##args)
#else
#define LOGI(fmt, args...) 
#endif

int main(int argc, char* argv[])
{
    LOGI(">>main\n");

    LOGI("HelloWorld\n");

    LOGI("<<main\n");

    return 0;
}
