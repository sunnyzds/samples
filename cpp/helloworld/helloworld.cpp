#include <stdio.h>

#include "log.h"

//#define printlog(tag, fmt, args...) printf("[%s]"fmt, tag, ##args)

int main(int argc, char* argv[])
{
	LOGI(">>main\n");

//	LOGD("Hello World\n");
	
	printlog("hello", "world");

//	LOGI("<<main\n");
	
	return 0;
}
