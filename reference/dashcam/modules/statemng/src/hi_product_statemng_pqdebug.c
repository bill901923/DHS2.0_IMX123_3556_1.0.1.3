/**
 * @file      hi_product_statemng_pqdebug.c
 * @brief     Realize the interface about Base state.
 *
 * Copyright (c) 2019 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/06/04
 * @version
 */

#include "hi_product_statemng_pqdebug.h"
#include "hi_product_statemng.h"
#include "hi_product_statemng_inner.h"
#include "hi_product_scene.h"
#include <errno.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

//#ifdef CONFIG_PQT_SUPPORT_ON

#define PDT_DEFPQSTART_PATH HI_APPFS_PATH"/bin/StartControl.sh"
#define PDT_DEFPQSTOP_PATH HI_APPFS_PATH"/bin/HiIspTool.sh -as"

HI_S32 PDT_STATEMNG_SetPQDebug(HI_BOOL state)
{
    HI_PDT_MEDIA_CFG_S mediaCfg;
    HI_S32 result = PDT_STATEMNG_GetMediaCfg(&mediaCfg);
    PDT_STATEMNG_CHECK_RET(result, result, "get media cfg");
#if defined(AMP_LINUX_HUAWEILITE)
    HI_S32  respRet = HI_SUCCESS;
    /** send message to liteOS to start/stop pq_control **/
    result = HI_MSG_SendSync(0,HI_MSGID_STATEMNG_PQDEBUG, &state, sizeof(HI_BOOL), NULL, &respRet, sizeof(respRet));
    PDT_STATEMNG_CHECK_RET(result, result, "send pq_control message to liteOS");
    PDT_STATEMNG_CHECK_RET(respRet, respRet, "liteOS pq_control");
#else
    HI_CHAR* cmd = (state == HI_TRUE)?HI_SHAREFS_ROOT_PATH:"stop";
    extern HI_VOID app_control(HI_S32 argc, HI_CHAR **argv );
    app_control(1, &cmd);
#endif

    if (state == HI_TRUE) {
        result = PDT_STATEMNG_AddRtspStreamsByType(&mediaCfg, HI_PDT_MEDIA_VENC_TYPE_REC);
        PDT_STATEMNG_CHECK_RET(result, result, "add Rtsp Streams[REC]");
#if defined(AMP_LINUX_HUAWEILITE)
        /** start the StartControl.sh **/
        result = HI_system(PDT_DEFPQSTART_PATH);
        if(result == HI_FAILURE) {
            MLOGE("system errno(%d)\n",errno);
        }
#endif
    } else {
        /** remove RtspStreams for HI_PDT_MEDIA_VENC_TYPE_REC */
        result = PDT_STATEMNG_RemoveRtspStreamsByType(&mediaCfg, HI_PDT_MEDIA_VENC_TYPE_REC);
        PDT_STATEMNG_CHECK_RET(result, result, "remove Rtsp Streams[REC]");
#if defined(AMP_LINUX_HUAWEILITE)
        /** do StartControl.sh  **/
        result = HI_system(PDT_DEFPQSTOP_PATH);
        if(result == HI_FAILURE) {
            MLOGE("system errno(%d)\n",errno);
        }
#endif
    }
    return HI_SUCCESS;
}

HI_S32 PDT_STATEMNG_HandlePQDebugMsg(HI_MESSAGE_S* msg)
{
    PDT_STATEMNG_CONTEXT* stateMngCtx = PDT_STATEMNG_GetCtx();
    HI_BOOL status;
    memcpy(&status, msg->aszPayload, sizeof(HI_BOOL));
    if (status == stateMngCtx->pqDebugStatus) {
        PDT_STATEMNG_CHECK_RET_AND_UPDATESTATUS(msg, HI_FAILURE, HI_SUCCESS, " check PQDebugStatus");
    }

    HI_S32 result = PDT_STATEMNG_SetPQDebug(status);
    if (result != HI_SUCCESS) {
        MLOGE("set PQDebug status[%d] failed\n", status);
        return result;
    }

    if (status == HI_TRUE) {
        (HI_VOID)HI_PDT_USBCTRL_SetMode(HI_USB_MODE_CHARGE);
    }

    stateMngCtx->pqDebugStatus = status;
    PDT_STATEMNG_UPDATESTATUS(msg, HI_SUCCESS, HI_FALSE);
    return HI_SUCCESS;
}

HI_S32 PDT_STATEMNG_AddPQDebugStreams(HI_VOID)
{
    HI_S32 result = HI_SUCCESS;
    PDT_STATEMNG_CONTEXT* stateMngCtx = PDT_STATEMNG_GetCtx();

    if (stateMngCtx->pqDebugStatus == HI_TRUE) {
        HI_PDT_MEDIA_CFG_S mediaCfg;
        result = PDT_STATEMNG_GetMediaCfg(&mediaCfg);
        if (result != HI_SUCCESS) {
            MLOGE(RED"get media config failed\n\n"NONE);
            return result;
        }
        result = PDT_STATEMNG_AddRtspStreamsByType(&mediaCfg, HI_PDT_MEDIA_VENC_TYPE_REC);
    }

    return result;
}
//#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
