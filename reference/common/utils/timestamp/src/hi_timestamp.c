/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: timestamp log functions.
 * Author: Camera Reference Develop Team
 * Create: 2017-12-18
 * History: 2017-12-18 create this file
 *          2019-06-28 add HI_TimeStamp_Init, adapt timer cksel
 */

#include <string.h>
#include <pthread.h>
#include "hi_appcomm_util.h"
#include "hi_timestamp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#if (defined(TIMER_REG_BASE) && defined(SC_CTRL))
/* defined in SoC Data Sheet */
#define TIMER_REG_VALUE   (TIMER_REG_BASE + 0x004)
#define TIMER_REG_CONTROL (TIMER_REG_BASE + 0x008)
#ifndef TIMER_CLK_MASK
#define TIMER_CLK_MASK (1 << 18) /* timeren2ov */
#endif
#define BUS_CLK   50000000 /* sysapb_cksel */
#define TIMER_CLK 3000000  /* timer default cksel */

#define TIME_STAMP_MAX_CNT (200)
#define TIME_STAMP_LEN     (16)

#define TIME_STAMP_CNT      (g_timestampCtx.phyMemAddr)
#define TIME_STAMP_VALUE    (TIME_STAMP_CNT + 0x10)
#define GET_MS_CLOCK(value) ((HI_U32)((HI_U64)value * g_timestampCtx.timerpre / (g_timestampCtx.timerClock / 1000)))

/** TimeStamp Context */
typedef struct {
    HI_U64 phyMemAddr;
    HI_U32 timerClock;
    HI_U32 timerpre;
    pthread_mutex_t mutex;
} TimeStamp_Context;

static TimeStamp_Context g_timestampCtx = {
    .phyMemAddr = 0,
    .timerClock = BUS_CLK,
    .timerpre = 256,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
};
#endif

HI_S32 HI_TimeStamp_Init(HI_U64 phyMemAddr, HI_BOOL cleanMem)
{
#if (defined(TIMER_REG_BASE) && defined(SC_CTRL))
    if (phyMemAddr == 0) {
        return HI_EINVAL;
    }
    /* get timer control */
    HI_U32 timerctrl = 0;
    himd(TIMER_REG_CONTROL, &timerctrl);
    if ((timerctrl & (1 << 7)) == 0) {
        MLOGE("Timer is not enable.\n");
        return HI_ENOTREADY;
    }
    HI_MUTEX_LOCK(g_timestampCtx.mutex);
    if (g_timestampCtx.phyMemAddr != 0) {
        HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
        return HI_EINITIALIZED;
    }
    g_timestampCtx.phyMemAddr = phyMemAddr;
    if (cleanMem == HI_TRUE) {
        himm((HI_U32)TIME_STAMP_CNT, 0);
    }
    /* get timer clock */
    HI_U32 timerenov = 1;
    himd(SC_CTRL, &timerenov);
    timerenov = timerenov & TIMER_CLK_MASK;
    g_timestampCtx.timerClock = (timerenov == 0) ? TIMER_CLK : BUS_CLK;
    /* get timer timerpre */
    switch (timerctrl & 0xc) {
        case 0x0:
            g_timestampCtx.timerpre = 1;
            break;
        case 0x4:
            g_timestampCtx.timerpre = 16;
            break;
        case 0x8:
        case 0xc:
            g_timestampCtx.timerpre = 256;
            break;
    }
    HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
#endif
    return HI_SUCCESS;
}

HI_S32 HI_TimeStamp_Mark(const HI_CHAR *func, HI_U32 line, HI_U32 type)
{
#if (defined(TIMER_REG_BASE) && defined(SC_CTRL))
    HI_U32 stamp = 0;
    himd((HI_UL)TIMER_REG_VALUE, &stamp);
    HI_MUTEX_LOCK(g_timestampCtx.mutex);
    if (g_timestampCtx.phyMemAddr == 0) {
        HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
        return HI_ENOINIT;
    }
    HI_U32 cnt = 0;
    HI_U32 *addr = NULL;
    himd(TIME_STAMP_CNT, &cnt);
    if (cnt > TIME_STAMP_MAX_CNT) {
        himm((HI_U32)TIME_STAMP_CNT, 0);
        cnt = 0;
        MLOGW("\n time_stamp is full!!! now clean the buffer to start from 0\n");
    }
    stamp = ~stamp;
    addr = (HI_U32 *)(HI_UL)(TIME_STAMP_VALUE + cnt * TIME_STAMP_LEN);
    himm((HI_UL)addr, stamp);
    addr++;
    himm((HI_UL)addr, (HI_UL)func);
    addr++;
    himm((HI_UL)addr, line);
    addr++;
    himm((HI_UL)addr, type);
    cnt++;
    himm(TIME_STAMP_CNT, cnt);
    HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
#endif
    return HI_SUCCESS;
}

HI_S32 HI_TimeStamp_Clean(HI_VOID)
{
#if (defined(TIMER_REG_BASE) && defined(SC_CTRL))
    HI_MUTEX_LOCK(g_timestampCtx.mutex);
    if (g_timestampCtx.phyMemAddr == 0) {
        HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
        return HI_ENOINIT;
    }
    himm((HI_U32)TIME_STAMP_CNT, 0);
    HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
#endif
    return HI_SUCCESS;
}

HI_S32 HI_TimeStamp_Print(HI_U32 type)
{
#if (defined(TIMER_REG_BASE) && defined(SC_CTRL))
    HI_MUTEX_LOCK(g_timestampCtx.mutex);
    if (g_timestampCtx.phyMemAddr == 0) {
        HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
        MLOGE("TimeStamp is not init.\n");
        return HI_ENOINIT;
    }
    HI_U32 cnt = 0;
    himd(TIME_STAMP_CNT, &cnt);
    HI_U32 Time_0 = 0;
    HI_U32 *addr = NULL;
    HI_U32 stamp = 0;
    HI_U32 i = 0;
    for (i = 0; i < cnt; i++) {
        addr = (HI_U32 *)(HI_UL)(TIME_STAMP_VALUE + i * TIME_STAMP_LEN);
        HI_U32 nType = 0;
        HI_U32 nTime = 0;
#ifdef __HuaweiLite__
        HI_U32 func = 0;
#else
        HI_CHAR *func = 0;
#endif
        HI_U32 nLine = 0;
        himd ((HI_UL)(addr + 3), &nType);
        himd ((HI_UL)(addr + 0), &nTime);
        nTime = GET_MS_CLOCK(nTime);
        himd ((HI_UL)(addr + 2), &nLine);
        himd ((HI_UL)(addr + 1), (HI_U32 *)&func);
        if (type == 0 || nType == type) {
            if (func != 0) {
                printf("time stamp[%-3d] = %-8dms gap = %-8dms type: %-5d  line: %-5d func: %s\n",
                       i + 1, nTime, nTime - stamp, nType, nLine, (HI_CHAR *)func);
            } else {
                printf("time stamp[%-3d] = %-8dms gap = %-8dms type: %-5d  line: %-5d\n",
                       i + 1, nTime, nTime - stamp, nType, nLine);
            }
        }
        stamp = nTime;
        if (i == 0) {
            Time_0 = nTime;
        }
        if (i == (cnt - 1)) {
            printf("total time from stamp[%-3d] to [%-3d]  gap = %-8dms\n", 1, i + 1, nTime - Time_0);
        }
    }
    HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
#else
    MLOGW("TIMER_REG_BASE or SC_CTRL is undefined.\n");
#endif
    return HI_SUCCESS;
}

HI_U32 HI_TimeStamp_Getms(HI_VOID)
{
#if (defined(TIMER_REG_BASE) && defined(SC_CTRL))
    HI_U32 curTime = 0;
    himd(TIMER_REG_VALUE, &curTime);
    HI_MUTEX_LOCK(g_timestampCtx.mutex);
    if (g_timestampCtx.phyMemAddr == 0) {
        HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
        return 0;
    }
    curTime = ~curTime;
    curTime = GET_MS_CLOCK(curTime);
    HI_MUTEX_UNLOCK(g_timestampCtx.mutex);
    return curTime;
#endif
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
