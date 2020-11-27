#ifndef SAMPLE_LOG_H
#define SAMPLE_LOG_H
#include <stdio.h>
#include <time.h>
#include "hi_type.h"

#ifdef _WIN32
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#else
#include <linux/limits.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define STR_VAL(name)   #name
#define TOSTR(name)   STR_VAL(name)
#ifdef CPU_AFFINITY
#define CPU_TASK_AFFINITY TOSTR(CPU_AFFINITY)
#else
#define CPU_TASK_AFFINITY ""
#endif

#ifndef SAMPLE_LOG_INFO
#define  SAMPLE_LOG_INFO(...) \
    do \
    { \
        printf(__VA_ARGS__); \
    } while (0)
#else
#define  SAMPLE_LOG_INFO(...)
#endif

#ifndef SAMPLE_CHK_GOTO
#define SAMPLE_CHK_GOTO(val, label, ...) \
    do \
    { \
        if ((val)) \
        { \
            SAMPLE_LOG_INFO(__VA_ARGS__); \
            goto label; \
        } \
    } while (0)
#endif

#ifndef SAMPLE_CHK_PRINTF
#define SAMPLE_CHK_PRINTF(val, ...) \
    do \
    { \
        if ((val)) \
        { \
            SAMPLE_LOG_INFO(__VA_ARGS__); \
        } \
    } while (0)
#endif

#ifndef SAMPLE_CHK_RET
#define SAMPLE_CHK_RET(val, ret, ...) \
    do \
    { \
        if ((val)) \
        { \
            SAMPLE_LOG_INFO(__VA_ARGS__); \
            return (ret); \
        } \
    } while (0)
#endif

#ifndef SAMPLE_CHK_RET_VOID
#define SAMPLE_CHK_RET_VOID(val, ...) \
    do \
    { \
        if ((val)) \
        { \
            SAMPLE_LOG_INFO(__VA_ARGS__); \
            return; \
        } \
    } while (0)
#endif


#ifdef _WIN32
    int clock_gettime(int, struct timespec* ct);
#endif

HI_VOID timeSpendMs(struct timespec* ptime1, struct timespec* ptime2, const char* des);
HI_VOID timePrint(struct timespec* ptime, const char* des);
HI_VOID printDebugData(const HI_CHAR* pcName, HI_U64 u64VirAddr, HI_U32 u32PrintLine);

#ifdef __cplusplus
}
#endif
#endif
