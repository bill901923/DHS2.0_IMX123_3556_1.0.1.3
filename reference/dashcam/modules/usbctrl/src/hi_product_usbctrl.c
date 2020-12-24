/**
 * @file      hi_product_usbctrl.c
 * @brief     usb control interface implementation
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/26
 * @version   1.0

 */

#include <pthread.h>

#include "hi_product_param.h"
#include "hi_product_statemng.h"
#include "hi_product_usbctrl.h"
#include "hi_eventhub.h"
#ifndef __HuaweiLite__
#include "hi_appcomm_util.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/** redefine module name */
#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE "USBCTRL"

/** usbcontrol context */
typedef struct tagPDT_USBCTRL_CONTEXT_S
{
    HI_MW_PTR pSubscriber;
    pthread_mutex_t EventMutex;
    pthread_cond_t  EventCond;
    HI_S32 s32WaitedValue; /* Waited Value */
    HI_BOOL pause;
    HI_BOOL waitedFlag;
    HI_USB_MODE_E prevMode;
    pthread_mutex_t pauseMutex;
} PDT_USBCTRL_CONTEXT_S;
static PDT_USBCTRL_CONTEXT_S s_stUSBCTRLCtx;

static inline const HI_CHAR* PDT_USBCTRL_GetEventStr(HI_S32 enEvent)
{
    if (HI_EVENT_USB_OUT == enEvent)
    {
        return "USB Out Event";
    }
    else if (HI_EVENT_USB_INSERT == enEvent)
    {
        return "USB Insert Event";
    }
    else if (HI_EVENT_USB_UVC_READY == enEvent)
    {
        return "UVC Ready Event";
    }
    else if (HI_EVENT_USB_STORAGE_READY == enEvent)
    {
        return "USB Storage Ready Event";
    }
    else
    {
        return "Unknown Usb Event";
    }
}

static HI_S32 PDT_USBCTRL_SubscribeEventProc(HI_EVENT_S* pstEvent, HI_VOID* pvPrivData)
{
    HI_APPCOMM_CHECK_POINTER(pstEvent, HI_FAILURE);
    MLOGI("Event[%08x], State[%d], WaitedValue=%d\n", pstEvent->EventID, pstEvent->arg2, s_stUSBCTRLCtx.s32WaitedValue);

    HI_MUTEX_LOCK(s_stUSBCTRLCtx.pauseMutex);
    if (pstEvent->arg2 == s_stUSBCTRLCtx.s32WaitedValue){
        s_stUSBCTRLCtx.waitedFlag = HI_FALSE;
    }
    HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);

    HI_MUTEX_LOCK(s_stUSBCTRLCtx.EventMutex);
    if (pstEvent->arg2 == s_stUSBCTRLCtx.s32WaitedValue){
        HI_COND_SIGNAL(s_stUSBCTRLCtx.EventCond);
        s_stUSBCTRLCtx.s32WaitedValue = -1;
    }
    HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.EventMutex);
    return HI_SUCCESS;
}

static HI_S32 PDT_USBCTRL_SubscribeEvent(PDT_USBCTRL_CONTEXT_S* pstCtx)
{
    HI_SUBSCRIBER_S stSubscriber = {"USBCtrl", PDT_USBCTRL_SubscribeEventProc, NULL, HI_TRUE};
    HI_EVTHUB_CreateSubscriber(&stSubscriber, &pstCtx->pSubscriber);
    MLOGD("subscribe create success[%p]\n", pstCtx->pSubscriber);

    HI_EVTHUB_Subscribe(pstCtx->pSubscriber, HI_EVENT_STATEMNG_SWITCH_WORKMODE);
    MLOGD("subscribe event %08x success\n", HI_EVENT_STATEMNG_SWITCH_WORKMODE);
    return HI_SUCCESS;
}

static HI_S32 PDT_USBCTRL_EventProc(const HI_USB_EVENT_INFO_S *pstEventInfo)
{
    HI_EVENT_S stEvent;
    switch (pstEventInfo->s32EventId)
    {
        case HI_EVENT_USB_OUT:
        {
            MLOGI("%s\n", PDT_USBCTRL_GetEventStr(pstEventInfo->s32EventId));
            stEvent.EventID = HI_EVENT_USB_OUT;
            HI_EVTHUB_Publish(&stEvent);
            break;
        }
        case HI_EVENT_USB_INSERT:
        {
            MLOGI("%s\n", PDT_USBCTRL_GetEventStr(pstEventInfo->s32EventId));
            stEvent.EventID = HI_EVENT_USB_INSERT;
            HI_EVTHUB_Publish(&stEvent);
            break;
        }
        case HI_EVENT_USB_UVC_READY:{
            MLOGI("s_stUSBCTRLCtx.pause=%d\n", s_stUSBCTRLCtx.pause);
            HI_MUTEX_LOCK(s_stUSBCTRLCtx.pauseMutex);
            if(s_stUSBCTRLCtx.pause == HI_FALSE) {
                s_stUSBCTRLCtx.waitedFlag = HI_TRUE;
                HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);

                MLOGI("s32EventId=%s\n", PDT_USBCTRL_GetEventStr(pstEventInfo->s32EventId));
                stEvent.EventID = HI_EVENT_USB_UVC_READY;

                MLOGD("Wait uvc workmode event...\n");
                HI_MUTEX_LOCK(s_stUSBCTRLCtx.EventMutex);
                HI_EVTHUB_Publish(&stEvent);

                s_stUSBCTRLCtx.s32WaitedValue = HI_PDT_WORKMODE_UVC;

                HI_COND_WAIT(s_stUSBCTRLCtx.EventCond, s_stUSBCTRLCtx.EventMutex);
                HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.EventMutex);
                MLOGD("Wait OK\n");
            } else {
                MLOGW("ignore UVC_READY proc\n");
                HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);
            }
            break;
        }

        case HI_EVENT_USB_STORAGE_READY:{
            MLOGI("s_stUSBCTRLCtx.pause=%d\n", s_stUSBCTRLCtx.pause);
            HI_MUTEX_LOCK(s_stUSBCTRLCtx.pauseMutex);
            if(s_stUSBCTRLCtx.pause == HI_FALSE) {
                MLOGI("s32EventId=%s\n", PDT_USBCTRL_GetEventStr(pstEventInfo->s32EventId));
                s_stUSBCTRLCtx.waitedFlag = HI_TRUE;
                HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);

                stEvent.EventID = HI_EVENT_USB_STORAGE_READY;

                MLOGI("Wait usb storage workmode event...\n");
                HI_MUTEX_LOCK(s_stUSBCTRLCtx.EventMutex);
                HI_EVTHUB_Publish(&stEvent);

                s_stUSBCTRLCtx.s32WaitedValue = HI_PDT_WORKMODE_USB_STORAGE;
                HI_COND_WAIT(s_stUSBCTRLCtx.EventCond, s_stUSBCTRLCtx.EventMutex);
                HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.EventMutex);
                MLOGI("Wait OK\n");
            } else {
                MLOGW("ignore STORAGE_READY proc\n");
                HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);
            }
            break;
        }

        default:
            MLOGW("Invalid Event[%08x]\n", pstEventInfo->s32EventId);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 HI_PDT_USBCTRL_RegisterEvent(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret  = HI_EVTHUB_Register(HI_EVENT_USB_OUT);
    s32Ret |= HI_EVTHUB_Register(HI_EVENT_USB_INSERT);
    s32Ret |= HI_EVTHUB_Register(HI_EVENT_USB_UVC_READY);
    s32Ret |= HI_EVTHUB_Register(HI_EVENT_USB_STORAGE_READY);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);
    MLOGD("Success\n");
    return HI_SUCCESS;
}

static HI_S32 PDT_USBCTRL_GetStorageState(HI_VOID* pvPrivData)
{
    if(NULL == pvPrivData)
    {
        return HI_FAILURE;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL* pbStorageReady = (HI_BOOL*)pvPrivData;
    *pbStorageReady = HI_FALSE;

    HI_STORAGEMNG_CFG_S stStorageMngCfg;
    s32Ret = HI_PDT_PARAM_GetStorageCfg(&stStorageMngCfg);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);

    /** check sd state */
    HI_STORAGE_STATE_E enState = HI_STORAGE_STATE_IDEL;
    s32Ret = HI_STORAGEMNG_GetState(stStorageMngCfg.szMntPath, &enState);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);
    MLOGI(YELLOW"storage state(%d)\n"NONE, enState);

    if(HI_STORAGE_STATE_MOUNTED == enState)
    {
        *pbStorageReady = HI_TRUE;
    }

    return HI_SUCCESS;
}
#ifndef __HuaweiLite__
static HI_VOID PDT_USBCTRL_LoadUSBPhy(HI_VOID)
{
    HI_insmod(HI_APPFS_KOMOD_PATH"/dwc3.ko",NULL);
}
#endif
HI_S32 HI_PDT_USBCTRL_Init(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_USB_CFG_S stUsbCfg;
    stUsbCfg.pfnEventProc = PDT_USBCTRL_EventProc;
    stUsbCfg.pfnGetStorageState = PDT_USBCTRL_GetStorageState;

    /* uvc configure */
    HI_PDT_WORKMODE_CFG_S stWorkModeCfg;
    HI_PDT_WORKMODE_E enWorkMode = HI_PDT_WORKMODE_UVC;
#ifndef __HuaweiLite__
    PDT_USBCTRL_LoadUSBPhy();
#endif
    s32Ret = HI_PDT_PARAM_GetWorkModeCfg(enWorkMode, &stWorkModeCfg);
    HI_APPCOMM_CHECK_RETURN_WITH_ERRINFO(s32Ret, HI_FAILURE, "GetUVCAttr");
    memcpy(&stUsbCfg.stUvcCfg, &stWorkModeCfg.unModeCfg.stUvcCfg.stUvcCfg, sizeof(HI_UVC_CFG_S));

    /* usb storage configure */
    enWorkMode = HI_PDT_WORKMODE_USB_STORAGE;
    s32Ret = HI_PDT_PARAM_GetWorkModeCfg(enWorkMode, &stWorkModeCfg);
    HI_APPCOMM_CHECK_RETURN_WITH_ERRINFO(s32Ret, HI_FAILURE, "GetUSBStorageCfg");
    memcpy(&stUsbCfg.stStorageCfg, &stWorkModeCfg.unModeCfg.stUsbStorageCfg, sizeof(HI_USB_STORAGE_CFG_S));

    /* subscribe event */
    PDT_USBCTRL_SubscribeEvent(&s_stUSBCTRLCtx);
    HI_MUTEX_INIT_LOCK(s_stUSBCTRLCtx.EventMutex);
    HI_COND_INIT(s_stUSBCTRLCtx.EventCond);
    HI_MUTEX_INIT_LOCK(s_stUSBCTRLCtx.pauseMutex);

    s32Ret = HI_USB_Init(&stUsbCfg);
    HI_APPCOMM_CHECK_RETURN_WITH_ERRINFO(s32Ret, s32Ret,"InitUsb");

    HI_USB_MODE_E enUsbMode;
    enUsbMode = HI_USB_MODE_CHARGE;
    s32Ret = HI_USB_SetMode(enUsbMode);
    s_stUSBCTRLCtx.prevMode = HI_USB_MODE_CHARGE;
    HI_APPCOMM_CHECK_RETURN_WITH_ERRINFO(s32Ret, s32Ret,"SetUsbMode");

    return HI_SUCCESS;
}

HI_S32 HI_PDT_USBCTRL_Deinit(HI_VOID)
{
    return HI_USB_Deinit();
}

HI_S32 HI_PDT_USBCTRL_Pause(HI_BOOL enable)
{
    HI_S32 ret = HI_FAILURE;

    HI_MUTEX_LOCK(s_stUSBCTRLCtx.pauseMutex);
    if(enable == s_stUSBCTRLCtx.pause) {
        HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);
        return HI_SUCCESS;
    } else if(enable && s_stUSBCTRLCtx.waitedFlag) {
        MLOGW("Could not pause USB\n");
        s_stUSBCTRLCtx.pause = HI_FALSE;
        HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);
        return HI_PDT_USBCTRL_EBUSY;
    }
    MLOGI("$$$$$$$$$$$$$$ enable=%d $$$$$$$$$$$$\n", enable);
    if(enable) {
        ret = HI_USB_SetMode(HI_USB_MODE_CHARGE);
    } else {
        ret = HI_USB_SetMode(s_stUSBCTRLCtx.prevMode);
    }
    s_stUSBCTRLCtx.pause = enable;
    HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);

    return ret;
}

HI_S32 HI_PDT_USBCTRL_SetMode(HI_USB_MODE_E mode)
{
    HI_S32 ret = 0;

    HI_MUTEX_LOCK(s_stUSBCTRLCtx.pauseMutex);
    if(s_stUSBCTRLCtx.pause) {
        MLOGW("USB ctrl paused\n");
        HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);
        return HI_PDT_USBCTRL_EBUSY;
    }
    ret = HI_USB_SetMode(mode);
    s_stUSBCTRLCtx.prevMode = mode;
    HI_MUTEX_UNLOCK(s_stUSBCTRLCtx.pauseMutex);
    /* ret = HI_SUCCESS = USB_SET_MODE_SUCCESS */
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

