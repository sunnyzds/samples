#include <stdio.h>
#include "util.h"

float cur_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (float)(tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0);
}
