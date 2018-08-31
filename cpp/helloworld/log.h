#ifndef _LOG_H_
#define _LOG_H_

#ifndef LOG_TAG
#define LOG_TAG ""
#endif

#define LOG_VERBOSE 0
#define LOG_INFO    1
#define LOG_DEBUG   2
#define LOG_ERROR   3

#ifndef LOG_NDEBUG
#define LOG_NDEBUG 1
#endif

#ifndef LOG_NDDEBUG
#define LOG_NDDEBUG 0
#endif

#ifndef LOG_NIDEBUG 
#define LOG_NIDEBUG 0
#endif

#define printlog(tag, fmt, args...) printf("[%s]:"fmt, tag, ##args)
//#define printlog(tag, fmt, args...) printf(fmt, ##args)

#ifndef LOG
#define LOG(priority, tag, ...) \
    ({ \
        if (((priority == LOG_VERBOSE) && (LOG_NDEBUG == 0)) ||  \
            ((priority == LOG_INFO) && (LOG_NIDEBUG == 0)) ||       \
            ((priority == LOG_DEBUG) && (LOG_NDDEBUG == 0))  ||   \
            (priority == LOG_ERROR)) {                                         \
                (void)printlog(tag, __VA_ARGS__);                          \
            }                                                                              \
    })
#endif

#ifndef LOGV
#define LOGV(...) LOG(LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#endif

#ifndef LOGD
#define LOGD(...) LOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif 

#ifndef LOGI
#define LOGI(...) LOG(LOG_INFO, LOG_TAG, __VA_ARGS__)
#endif

#ifndef LOGE
#define LOGE(...) LOG(LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

#endif /*_LOG_H_*/
