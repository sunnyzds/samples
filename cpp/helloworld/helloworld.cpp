#include <stdio.h>

#define LOG_TAG "MAIN"
#include "log.h"

int main(int argc, char* argv[])
{
    LOGI(">>main\n");

    printlog("hello", "world\n");
    
    LOGI("<<main\n");

    return 0;
}
