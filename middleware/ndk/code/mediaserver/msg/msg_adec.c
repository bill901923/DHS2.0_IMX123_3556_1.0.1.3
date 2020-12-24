/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    msg_adec.c
 * @brief   msg adec function
 * @author  HiMobileCam NDK develop team
 * @date      2019/06/19
 */

#include "msg_adec.h"
#include "hi_comm_adec.h"
#include "hi_ipcmsg.h"
#include "hi_mapi_adec_define.h"
#include "hi_mapi_log.h"
#include "hi_mapi_adec.h"
#include "mapi_comm_inner.h"
#include "msg_server.h"
#include "msg_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static HI_S32 MSG_ADEC_Init(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE hAdecHdl;
    hAdecHdl = GET_DEV_ID(pstMsg->u32Module);
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;

    s32Ret = HI_MAPI_ADEC_Init(hAdecHdl, (HI_MAPI_ADEC_ATTR_S *)pstMsg->pBody);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MAPI_ADEC_Init fail: %#x\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, HI_NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "SendAsync fail,ret:[%#x]\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

static HI_S32 MSG_ADEC_Deinit(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE hAdecHdl;
    hAdecHdl = GET_DEV_ID(pstMsg->u32Module);
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg= HI_NULL;

    s32Ret = HI_MAPI_ADEC_Deinit(hAdecHdl);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MAPI_ADEC_Deinit fail: %#x\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, HI_NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "SendAsync fail,ret:[%#x]\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

static HI_S32 MSG_ADEC_SendStream(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE hAdecHdl;
    hAdecHdl = GET_DEV_ID(pstMsg->u32Module);
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_BOOL bBlock = (HI_BOOL)pstMsg->as32PrivData[0];

    s32Ret = HI_MAPI_ADEC_SendStream(hAdecHdl, (AUDIO_STREAM_S *)pstMsg->pBody, bBlock);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MAPI_ADEC_SendStream fail: %#x\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, HI_NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "SendAsync fail,ret:[%#x]\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

static HI_S32 MSG_ADEC_GetFrame(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE hAdecHdl;
    hAdecHdl = GET_DEV_ID(pstMsg->u32Module);
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_BOOL bBlock = (HI_BOOL)pstMsg->as32PrivData[0];

    s32Ret = HI_MAPI_ADEC_GetFrame(hAdecHdl, (HI_MAPI_AUDIO_FRAME_INFO_S *)pstMsg->pBody, bBlock);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MAPI_ADEC_GetFrame fail: %#x\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, HI_NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "SendAsync fail,ret:[%#x]\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

static HI_S32 MSG_ADEC_ReleaseFrame(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE hAdecHdl;
    hAdecHdl = GET_DEV_ID(pstMsg->u32Module);
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;

    s32Ret = HI_MAPI_ADEC_ReleaseFrame(hAdecHdl, (HI_MAPI_AUDIO_FRAME_INFO_S *)pstMsg->pBody);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MAPI_ADEC_ReleaseFrame fail: %#x\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, HI_NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "SendAsync fail,ret:[%#x]\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

static HI_S32 MSG_ADEC_SendEndOfStream(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE hAdecHdl;
    hAdecHdl = GET_DEV_ID(pstMsg->u32Module);
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;

    s32Ret = HI_MAPI_ADEC_SendEndOfStream(hAdecHdl);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MAPI_ADEC_SendEndOfStream fail: %#x\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, HI_NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "SendAsync fail,ret:[%#x]\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

static MSG_MODULE_CMD_S g_stModuleCmdTable[] = {
    { MSG_CMD_ADEC_INIT,             MSG_ADEC_Init },
    { MSG_CMD_ADEC_DEINIT,           MSG_ADEC_Deinit },
    { MSG_CMD_ADEC_SEND_STREAM,      MSG_ADEC_SendStream },
    { MSG_CMD_ADEC_GET_FRAME,        MSG_ADEC_GetFrame },
    { MSG_CMD_ADEC_RELEASE_FRAME,    MSG_ADEC_ReleaseFrame },
    { MSG_CMD_ADEC_SEND_ENDOFSTREAM, MSG_ADEC_SendEndOfStream },
};

MSG_SERVER_MODULE_S g_stModuleADEC = {
    HI_MAPI_MOD_ADEC,
    "adec",
    sizeof(g_stModuleCmdTable) / sizeof(MSG_MODULE_CMD_S),
    &g_stModuleCmdTable[0]
};

MSG_SERVER_MODULE_S *MAPI_MSG_GetAdecMod(HI_VOID)
{
    return &g_stModuleADEC;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
