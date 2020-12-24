/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    msg_client.c
 * @brief   msg client function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */
#include "hi_ipcmsg.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "hi_mapi_log.h"
#include "hi_mapi_sys.h"
#include "hi_mapi_comm_define.h"
#include "hi_mapi_errno.h"

#include "msg_define.h"
#include "mapi_comm_inner.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static HI_S32 g_mMediaMsgId;
static pthread_t g_mediaReceivePid = -1;

static void MEDIA_MSG_HandleMessage(HI_S32 s32Id, HI_IPCMSG_MESSAGE_S *pstMsg)
{
   // HI_U32 u32ModID;
    HI_IPCMSG_MESSAGE_S *respMsg;
    HI_S32 s32Ret;

   // u32ModID = GET_MOD_ID(pstMsg->u32Module);
//GET_MOD_ID(pstMsg->u32Module);
    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, HI_SUCCESS, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_IPCMSG_CreateRespMessage error\n");
        return;
    }

    s32Ret = HI_IPCMSG_SendAsync(s32Id, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "SendAsync fail,ret:[%#x]\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return;
}

static void *ReceiveThread(void *arg)
{
    HI_S32 *pId = (HI_S32 *)arg;

    prctl(PR_SET_NAME, (unsigned long)"Hi_pTMsgRec", 0, 0, 0);
    MAPI_INFO_TRACE(HI_MAPI_MOD_SYS, "Run...\n");
    HI_IPCMSG_Run(*pId);
    MAPI_INFO_TRACE(HI_MAPI_MOD_SYS, "after Run...\n");
    return HI_NULL;
}

HI_S32 MAPI_MEDIA_MSG_Init(HI_VOID)
{
    HI_S32 s32Ret;
    HI_IPCMSG_CONNECT_S stConnectAttr = { 1, HI_IPCMSG_PORT_HIMPP, 1 };
    s32Ret = HI_IPCMSG_AddService("HiMPP_MSG", &stConnectAttr);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "client ipcmsg add service fail\n");
        return s32Ret;
    }

    /* connect with block to prevent tryconnect fail */
    s32Ret = HI_IPCMSG_Connect(&g_mMediaMsgId, "HiMPP_MSG", MEDIA_MSG_HandleMessage);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "Connect fail\n");
        return s32Ret;
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    s32Ret = pthread_create(&g_mediaReceivePid, &attr, ReceiveThread, &g_mMediaMsgId);
    if (s32Ret != HI_SUCCESS) {
        pthread_attr_destroy(&attr);
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "Media_MSG_Init pthread_create ReceiveThread fail\n");
        return s32Ret;
    }

    pthread_attr_destroy(&attr);
    return 0;
}

HI_S32 MAPI_MEDIA_MSG_Deinit(HI_VOID)
{
    HI_S32 s32Ret;

    s32Ret = HI_IPCMSG_Disconnect(g_mMediaMsgId);
    MAPI_DEBUG_TRACE(HI_MAPI_MOD_SYS, "HI_IPCMSG_Disconnect\n");

    HI_IPCMSG_DelService("HiMPP_MSG");

    MAPI_DEBUG_TRACE(HI_MAPI_MOD_SYS, "Media_MSG_DeInit\n");
    return s32Ret;
}

HI_S32 MAPI_MEDIA_MSG_GetSiId(HI_VOID)
{
    return g_mMediaMsgId;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
