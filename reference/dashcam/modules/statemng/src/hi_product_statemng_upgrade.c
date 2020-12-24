/**
 * @file      hi_product_statemng_upgrade.c
 * @brief     Realize the interface about Base state.
 *
 * Copyright (c) 2018 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/08/30
 * @version
 */
#include "hi_system.h"
#include "hi_product_statemng.h"
#include "hi_product_statemng_inner.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/**Upgrade Mode Abnormal*/
static HI_VOID PDT_STATEMNG_UpgradeAbnormal(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    PDT_STATEMNG_CONTEXT *pstStateMngCtx = PDT_STATEMNG_GetCtx();

    MUTEX_LOCK(pstStateMngCtx->Mutex);
    pstStateMngCtx->bRunning = HI_FALSE;
    pstStateMngCtx->bInProgress = HI_FALSE;
    MUTEX_UNLOCK(pstStateMngCtx->Mutex);

    /** publish HI_EVENT_STATEMNG_UPGRADE_ABNORMAL event to EventHub */
    HI_EVENT_S stEvent;
    memset(&stEvent, 0 , sizeof(HI_EVENT_S));
    stEvent.EventID = HI_EVENT_STATEMNG_UPGRADE_ABNORMAL;
    s32Ret = HI_EVTHUB_Publish(&stEvent);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"publish switch workmode failed(0x%08X)\n"NONE, s32Ret);
    }
}

/** Enter Upgrate state */
HI_S32 PDT_STATEMNG_UpgradeStateEnter(HI_VOID *pvArg)
{
    HI_S32 s32Ret = HI_SUCCESS;

    PDT_STATEMNG_CONTEXT *pstStateMngCtx = PDT_STATEMNG_GetCtx();
    /** check parameters */
    PDT_STATEMNG_CHECK_POINTER(pvArg,HI_PDT_STATEMNG_ENULLPTR,"parameter pvArg error");

    PDT_STATEMNG_STATE_ATTR_S *pstStateAttr = (PDT_STATEMNG_STATE_ATTR_S *)pvArg;
    MLOGD(YELLOW"try to enter state(%s), bRunning(%d)\n\n"NONE,
                pstStateAttr->stState.name, (pstStateAttr->bRunning));

    /** get enter workmode and check it */
    HI_PDT_WORKMODE_E enEnterWorkMode = HI_PDT_WORKMODE_BUTT;
    s32Ret = PDT_STATEMNG_State2WorkMode(pstStateAttr->stState.name, &enEnterWorkMode);
    PDT_STATEMNG_CHECK_RET(s32Ret,s32Ret,"change state to workmode");
    if(HI_PDT_WORKMODE_UPGRADE != enEnterWorkMode)
    {
        MLOGE(RED"enter workmode[%d] error\n\n"NONE,enEnterWorkMode);
        return HI_PDT_STATEMNG_EINVAL;
    }
    else
    {
        pstStateMngCtx->enCurrentWorkMode = enEnterWorkMode;
        pstStateMngCtx->bRunning = pstStateAttr->bRunning;
    }

    /** publish HI_EVENT_STATEMNG_SWITCH_WORKMODE event to EventHub */
    HI_EVENT_S stEvent;
    memset(&stEvent, 0 , sizeof(HI_EVENT_S));
    stEvent.EventID = HI_EVENT_STATEMNG_SWITCH_WORKMODE;
    stEvent.arg2 = enEnterWorkMode;
    stEvent.s32Result = HI_SUCCESS;

    MUTEX_LOCK(pstStateMngCtx->Mutex);
    pstStateMngCtx->bInProgress = HI_FALSE;
    MUTEX_UNLOCK(pstStateMngCtx->Mutex);

    s32Ret = HI_EVTHUB_Publish(&stEvent);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"publish HI_EVENT_STATEMNG_SWITCH_WORKMODE event failed\n\n"NONE);
        return HI_FAILURE;
    }
    else
    {
        MLOGD(YELLOW"publish event:HI_EVENT_STATEMNG_SWITCH_WORKMODE(%x), "
                    "arg2(%s) to EventHub succeed\n\n"NONE,
                    stEvent.EventID, pstStateAttr->stState.name);
    }

    return s32Ret;
}

/** Exit Upgrate state */
HI_S32 PDT_STATEMNG_UpgradeStateExit(HI_VOID *pvArg)
{
    /** check parameters */
    PDT_STATEMNG_CHECK_POINTER(pvArg,HI_PDT_STATEMNG_ENULLPTR,"parameter pvArg error");

    PDT_STATEMNG_STATE_ATTR_S *pstStateAttr = (PDT_STATEMNG_STATE_ATTR_S *)pvArg;
    MLOGD(YELLOW"exit state(%s), bRunning(%d)\n"NONE, pstStateAttr->stState.name, pstStateAttr->bRunning);

    return HI_SUCCESS;
}

/** Upgrate state message process */
HI_S32 PDT_STATEMNG_UpgradeStateMsgProc(HI_MESSAGE_S *pstMsg, HI_MW_PTR pvArg, HI_STATE_ID *pStateID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    PDT_STATEMNG_CONTEXT *pstStateMngCtx = PDT_STATEMNG_GetCtx();
    /** check parameters */
    PDT_STATEMNG_CHECK_MSGPROC_FUNC_PARAM(pvArg, pStateID, pstMsg,
                                          pstStateMngCtx->bInProgress);

    PDT_STATEMNG_STATE_ATTR_S *pstStateAttr = (PDT_STATEMNG_STATE_ATTR_S *)pvArg;
    MLOGD("current state(%s), bRunning(%d)\n\n",
          pstStateAttr->stState.name, (pstStateAttr->bRunning));

    HI_PDT_WORKMODE_E enCurrentWorkMode = HI_PDT_WORKMODE_BUTT;
    s32Ret = PDT_STATEMNG_State2WorkMode(pstStateAttr->stState.name, &enCurrentWorkMode);
    PDT_STATEMNG_CHECK_RET(s32Ret,HI_STORAGEMNG_EINTER,"change state to workmode");


    switch(pstMsg->what)
    {
        case HI_EVENT_STATEMNG_POWEROFF:
        {
            MLOGD(YELLOW"no need to process message what(%x)\n\n"NONE,
                        pstMsg->what);
            MUTEX_LOCK(pstStateMngCtx->Mutex);
            pstStateMngCtx->bInProgress = HI_FALSE;
            MUTEX_UNLOCK(pstStateMngCtx->Mutex);
            PDT_STATEMNG_PublishEvent(pstMsg, HI_SUCCESS);
            return HI_PROCESS_MSG_RESULTE_OK;
        }

        case HI_EVENT_STATEMNG_UPGRADE_START:
        {
            HI_EVENT_S stUpgradeEvent = {};
            HI_UPGRADE_EVENT_INFO_S* eventInfo;
            HI_S32 loopMaxCount = 10;
            HI_S32 loopIndex = 0;

            while (loopIndex < loopMaxCount){
                loopIndex++;
                MLOGI("try start board upgrade[%d]\n", loopIndex);

                s32Ret = HI_EVTHUB_GetEventHistory(HI_EVENT_UPGRADE_NEWPKT, &stUpgradeEvent);
                if (HI_SUCCESS == s32Ret){
                    /**upgrade*/
                    MLOGI("start board upgrade[%s]\n", stUpgradeEvent.aszPayload);
                    eventInfo = (HI_UPGRADE_EVENT_INFO_S *) stUpgradeEvent.aszPayload;
                    s32Ret = HI_UPGRADE_DoUpgrade(pstStateMngCtx->stStorageMngCfg.szMntPath, eventInfo->szPktFilePath);
                    if (HI_SUCCESS == s32Ret) {
                        MLOGI(YELLOW"Do Upgrade Success, try to reboot\n"NONE);
                        HI_SYSTEM_Reboot();
                    } else {
                        MLOGI("upgrade failed\n");
                        break;
                    }
                }
                HI_usleep(100*1000);
            }
            PDT_STATEMNG_UpgradeAbnormal();
            return HI_PROCESS_MSG_RESULTE_OK;
        }

        case HI_EVENT_STATEMNG_UPGRADE_FAILED:
        case HI_EVENT_STORAGEMNG_DEV_UNPLUGED:
        {
            PDT_STATEMNG_UpgradeAbnormal();
            return HI_PROCESS_MSG_RESULTE_OK;
        }

        default:
        {
            MLOGD(YELLOW"pload msg to Base state\n"NONE);
            return HI_PROCESS_MSG_UNHANDLER;
        }
    }

    return HI_PROCESS_MSG_RESULTE_OK;
}

/** Init Upgrate state */
HI_S32 PDT_STATEMNG_UpgradeStateInit(const PDT_STATEMNG_STATE_ATTR_S* pstBase)
{
    HI_S32 s32Ret;

    static PDT_STATEMNG_STATE_ATTR_S stUpgrade =
        {{HI_PDT_WORKMODE_UPGRADE,
          PDT_STATEMNG_STATE_UPGRADE,
          PDT_STATEMNG_UpgradeStateEnter,
          PDT_STATEMNG_UpgradeStateExit,
          PDT_STATEMNG_UpgradeStateMsgProc,
          NULL},HI_FALSE};
    stUpgrade.stState.argv = &stUpgrade;

    s32Ret = HI_HFSM_AddState(PDT_STATEMNG_GetCtx()->pvHfsmHdl,
        &stUpgrade.stState, (HI_STATE_S*)&(pstBase->stState));
    PDT_STATEMNG_CHECK_RET(s32Ret,HI_STORAGEMNG_EINTER,"Add Upgrade");

    return s32Ret;
}

/** Deinit Upgrate state */
HI_S32 PDT_STATEMNG_UpgradeStateDeinit(HI_VOID)
{
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

