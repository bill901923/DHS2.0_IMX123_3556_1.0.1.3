/**
 * @file    hi_timedtask.c
 * @brief   timed-task interface.
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/12
 * @version   1.0

 */
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "hi_timedtask.h"
#include "hi_defs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define INVALID_TIMER_GROUP (-1)

/** time task max number */
#define TIMEDTASK_MAX_NUM (16)

/** check interval in usec */
#define TASK_CHECK_INTERVAL (300000)

/** timed-task check manage struct */
typedef struct tagTIMEDTASK_MNG_S {
    HI_BOOL bUsed;
    HI_MW_PTR timerHdl;
    pthread_mutex_t mutex;
    HI_TIMEDTASK_CFG_S stCfg;
} TIMEDTASK_MNG_S;
static TIMEDTASK_MNG_S s_stTIMEDTSKMng[TIMEDTASK_MAX_NUM];

/** timer group id */
static HI_S32 g_timerGroup = INVALID_TIMER_GROUP;

/**
 * @brief    timed task module initialization.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/12
 */
HI_S32 HI_TIMEDTASK_Init(HI_VOID)
{
    HI_APPCOMM_CHECK_EXPR(g_timerGroup == INVALID_TIMER_GROUP, HI_EINITIALIZED);
    g_timerGroup = HI_Timer_Init(HI_FALSE);
    if (g_timerGroup == HI_FAILURE) {
        MLOGE("Create Timer Group Failed\n");
        return HI_FAILURE;
    }
    HI_Timer_SetTickValue(g_timerGroup, TASK_CHECK_INTERVAL);
    HI_S32 s32Idx = 0;
    for (s32Idx = 0; s32Idx < TIMEDTASK_MAX_NUM; ++s32Idx) {
        HI_MUTEX_INIT_LOCK(s_stTIMEDTSKMng[s32Idx].mutex);
        HI_MUTEX_LOCK(s_stTIMEDTSKMng[s32Idx].mutex);
        s_stTIMEDTSKMng[s32Idx].bUsed = HI_FALSE;
        memset(&s_stTIMEDTSKMng[s32Idx].stCfg, 0, sizeof(HI_TIMEDTASK_CFG_S));
        HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[s32Idx].mutex);
    }
    return HI_SUCCESS;
}

/**
 * @brief    timed task module deinitialization.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/12
 */
HI_S32 HI_TIMEDTASK_Deinit(HI_VOID)
{
    HI_APPCOMM_CHECK_EXPR(g_timerGroup != INVALID_TIMER_GROUP, HI_ENOINIT);
    HI_Timer_DeInit(g_timerGroup);
    g_timerGroup = INVALID_TIMER_GROUP;
    HI_S32 s32Idx = 0;
    for (s32Idx = 0; s32Idx < TIMEDTASK_MAX_NUM; ++s32Idx) {
        HI_MUTEX_DESTROY(s_stTIMEDTSKMng[s32Idx].mutex);
    }
    return HI_SUCCESS;
}

/**
 * @brief    create timed task.
 * @param[in] pstTimeTskCfg : timed task config.
 * @param[out] pTimeTskhdl : timed task handle.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/12
 */
HI_S32 HI_TIMEDTASK_Create(const HI_TIMEDTASK_CFG_S *pstTimeTskCfg, HI_HANDLE *pTimeTskhdl)
{
    HI_S32 s32Idx = 0;
    HI_APPCOMM_CHECK_EXPR(g_timerGroup != INVALID_TIMER_GROUP, HI_ENOINIT);
    HI_APPCOMM_CHECK_POINTER(pstTimeTskCfg, HI_EINVAL);
    HI_APPCOMM_CHECK_POINTER(pTimeTskhdl, HI_EINVAL);
    if (pstTimeTskCfg->stAttr.bEnable) {
        HI_APPCOMM_CHECK_EXPR(pstTimeTskCfg->stAttr.u32Time_sec != 0, HI_EINVAL);
    }
    HI_APPCOMM_CHECK_EXPR(pstTimeTskCfg->timerProc != 0, HI_EINVAL);
    for (s32Idx = 0; s32Idx < TIMEDTASK_MAX_NUM; ++s32Idx) {
        HI_MUTEX_LOCK(s_stTIMEDTSKMng[s32Idx].mutex);
        if (HI_FALSE == s_stTIMEDTSKMng[s32Idx].bUsed) {
            if (pstTimeTskCfg->stAttr.bEnable == HI_TRUE) {
                HI_TIMER_S timerConf;
                timerConf.now = NULL;
                timerConf.interval_ms = pstTimeTskCfg->stAttr.u32Time_sec * 1000;
                timerConf.periodic = pstTimeTskCfg->stAttr.periodic;
                timerConf.timer_proc = pstTimeTskCfg->timerProc;
                timerConf.clientData = pstTimeTskCfg->pvPrivData;
                s_stTIMEDTSKMng[s32Idx].timerHdl = HI_Timer_Create(g_timerGroup, &timerConf);
                if (s_stTIMEDTSKMng[s32Idx].timerHdl == NULL) {
                    HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[s32Idx].mutex);
                    MLOGE("Create Timer Task Failed\n");
                    return HI_FAILURE;
                }
            }
            memcpy(&s_stTIMEDTSKMng[s32Idx].stCfg, pstTimeTskCfg, sizeof(HI_TIMEDTASK_CFG_S));
            s_stTIMEDTSKMng[s32Idx].bUsed = HI_TRUE;
            *pTimeTskhdl = s32Idx;
            HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[s32Idx].mutex);
            return HI_SUCCESS;
        }
        HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[s32Idx].mutex);
    }
    return HI_ENORES;
}

/**
 * @brief    destroy timed task.
 * @param[in] TimeTskhdl : timed task handle.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/12
 */
HI_S32 HI_TIMEDTASK_Destroy(HI_HANDLE TimeTskhdl)
{
    HI_APPCOMM_CHECK_EXPR(g_timerGroup != INVALID_TIMER_GROUP, HI_ENOINIT);
    HI_APPCOMM_CHECK_EXPR(TimeTskhdl < TIMEDTASK_MAX_NUM, HI_EINVAL);
    HI_MUTEX_LOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
    if (HI_FALSE == s_stTIMEDTSKMng[TimeTskhdl].bUsed) {
        HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
        return HI_EINVAL;
    }
    if (s_stTIMEDTSKMng[TimeTskhdl].timerHdl != NULL) {
        HI_Timer_Destroy(g_timerGroup, s_stTIMEDTSKMng[TimeTskhdl].timerHdl);
        s_stTIMEDTSKMng[TimeTskhdl].timerHdl = NULL;
    }
    s_stTIMEDTSKMng[TimeTskhdl].bUsed = HI_FALSE;
    memset(&s_stTIMEDTSKMng[TimeTskhdl].stCfg, 0, sizeof(HI_TIMEDTASK_CFG_S));
    HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
    return HI_SUCCESS;
}

/**
 * @brief    get timed task attribute.
 * @param[in] TimeTskhdl : timed task handle.
 * @param[out] pstTimeTskCfg : timed task attribute.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/12
 */
HI_S32 HI_TIMEDTASK_GetAttr(HI_HANDLE TimeTskhdl, HI_TIMEDTASK_ATTR_S *pstTimeTskAttr)
{
    HI_APPCOMM_CHECK_EXPR(g_timerGroup != INVALID_TIMER_GROUP, HI_ENOINIT);
    HI_APPCOMM_CHECK_POINTER(pstTimeTskAttr, HI_EINVAL);
    HI_APPCOMM_CHECK_EXPR(TimeTskhdl < TIMEDTASK_MAX_NUM, HI_EINVAL);
    HI_MUTEX_LOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
    if (HI_FALSE == s_stTIMEDTSKMng[TimeTskhdl].bUsed) {
        HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
        return HI_EINVAL;
    }
    memcpy(pstTimeTskAttr, &s_stTIMEDTSKMng[TimeTskhdl].stCfg.stAttr, sizeof(HI_TIMEDTASK_ATTR_S));
    HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
    return HI_SUCCESS;
}

/**
 * @brief    set timed task attribute.
 * @param[in] TimeTskhdl : timed task handle.
 * @param[in] pstTimeTskCfg : timed task attribute.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/12
 */
HI_S32 HI_TIMEDTASK_SetAttr(HI_HANDLE TimeTskhdl, const HI_TIMEDTASK_ATTR_S *pstTimeTskAttr)
{
    HI_APPCOMM_CHECK_EXPR(g_timerGroup != INVALID_TIMER_GROUP, HI_ENOINIT);
    HI_APPCOMM_CHECK_POINTER(pstTimeTskAttr, HI_EINVAL);
    if (pstTimeTskAttr->bEnable) {
        HI_APPCOMM_CHECK_EXPR(pstTimeTskAttr->u32Time_sec != 0, HI_EINVAL);
    }
    HI_APPCOMM_CHECK_EXPR(TimeTskhdl < TIMEDTASK_MAX_NUM, HI_EINVAL);
    HI_MUTEX_LOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
    if (HI_FALSE == s_stTIMEDTSKMng[TimeTskhdl].bUsed) {
        HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
        return HI_EINVAL;
    }
    if (s_stTIMEDTSKMng[TimeTskhdl].timerHdl != NULL) {
        if (pstTimeTskAttr->bEnable) {
            struct timespec ts = { 0, 0 };
            clock_gettime(CLOCK_MONOTONIC, &ts);
            struct timeval now;
            now.tv_sec = ts.tv_sec;
            now.tv_usec = ts.tv_nsec / 1000;
            HI_Timer_Reset(g_timerGroup, s_stTIMEDTSKMng[TimeTskhdl].timerHdl, &now, (pstTimeTskAttr->u32Time_sec * 1000));
        } else {
            HI_Timer_Destroy(g_timerGroup, s_stTIMEDTSKMng[TimeTskhdl].timerHdl);
            s_stTIMEDTSKMng[TimeTskhdl].timerHdl = NULL;
        }
    } else {
        if (pstTimeTskAttr->bEnable) {
            HI_TIMER_S timerConf;
            timerConf.now = NULL;
            timerConf.interval_ms = pstTimeTskAttr->u32Time_sec * 1000;
            timerConf.periodic = pstTimeTskAttr->periodic;
            timerConf.timer_proc = s_stTIMEDTSKMng[TimeTskhdl].stCfg.timerProc;
            timerConf.clientData = s_stTIMEDTSKMng[TimeTskhdl].stCfg.pvPrivData;
            s_stTIMEDTSKMng[TimeTskhdl].timerHdl = HI_Timer_Create(g_timerGroup, &timerConf);
            if (s_stTIMEDTSKMng[TimeTskhdl].timerHdl == NULL) {
                HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
                MLOGE("Create Timer Task Failed\n");
                return HI_FAILURE;
            }
        }
    }
    memcpy(&s_stTIMEDTSKMng[TimeTskhdl].stCfg.stAttr, pstTimeTskAttr, sizeof(HI_TIMEDTASK_ATTR_S));
    HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
    return HI_SUCCESS;
}

/**
 * @brief    reset specified task check time.
 * @param[in] TimeTskhdl : timed task handle.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/12
 */
HI_S32 HI_TIMEDTASK_ResetTime(HI_HANDLE TimeTskhdl)
{
    HI_APPCOMM_CHECK_EXPR(g_timerGroup != INVALID_TIMER_GROUP, HI_ENOINIT);
    HI_APPCOMM_CHECK_EXPR(TimeTskhdl < TIMEDTASK_MAX_NUM, HI_EINVAL);
    HI_MUTEX_LOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
    if (HI_FALSE == s_stTIMEDTSKMng[TimeTskhdl].bUsed) {
        HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
        return HI_EINVAL;
    }
    if (s_stTIMEDTSKMng[TimeTskhdl].timerHdl != NULL) {
        struct timespec ts = { 0, 0 };
        clock_gettime(CLOCK_MONOTONIC, &ts);
        struct timeval now;
        now.tv_sec = ts.tv_sec;
        now.tv_usec = ts.tv_nsec / 1000;
        HI_Timer_Reset(g_timerGroup, s_stTIMEDTSKMng[TimeTskhdl].timerHdl, &now,
                       (s_stTIMEDTSKMng[TimeTskhdl].stCfg.stAttr.u32Time_sec * 1000));
    }
    HI_MUTEX_UNLOCK(s_stTIMEDTSKMng[TimeTskhdl].mutex);
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

