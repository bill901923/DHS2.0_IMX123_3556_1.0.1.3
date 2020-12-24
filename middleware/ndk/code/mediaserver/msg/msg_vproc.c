/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    msg_vproc.c
 * @brief   msg vproc function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */
#include "hi_type.h"
#include "hi_ipcmsg.h"
#include "msg_define.h"
#include "msg_vproc.h"
#include "hi_mapi_vproc_define.h"
#include "hi_mapi_vproc.h"
#include "mapi_vproc_inner.h"
#include "hi_mapi_log.h"
#include "msg_server.h"
#include "mapi_comm_inner.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static HI_S32 MSG_VPROC_InitVpss(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_InitVpss(VpssHdl, (HI_MAPI_VPSS_ATTR_S *)pstMsg->pBody);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_DeinitVpss(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_DeinitVpss(VpssHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_BindVcap(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VcapPipeHdl, PipeChnHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VcapPipeHdl = pstMsg->as32PrivData[0];
    PipeChnHdl = pstMsg->as32PrivData[1];

    s32Ret = HI_MAPI_VPROC_BindVcap(VcapPipeHdl, PipeChnHdl, VpssHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_UnbindVcap(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VcapPipeHdl, PipeChnHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VcapPipeHdl = pstMsg->as32PrivData[0];
    PipeChnHdl = pstMsg->as32PrivData[1];

    s32Ret = HI_MAPI_VPROC_UnBindVCap(VcapPipeHdl, PipeChnHdl, VpssHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}


static HI_S32 MSG_VPROC_StartRecvVcapYuv(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VcapPipeHdl;
    VI_YUV_DATA_SEQ_E enYuvDataSeq;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VcapPipeHdl = pstMsg->as32PrivData[0];
    enYuvDataSeq = pstMsg->as32PrivData[1];

    s32Ret = HI_MAPI_VPROC_StartRecvVcapYuv(VpssHdl, VcapPipeHdl, enYuvDataSeq);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_StopRecvVcapYuv(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_StopRecvVcapYuv(VpssHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}



static HI_S32 MSG_VPROC_VportSetAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_SetPortAttr(VpssHdl, VPortHdl, (HI_MAPI_VPORT_ATTR_S *)pstMsg->pBody);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_VportGetAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_GetPortAttr(VpssHdl, VPortHdl, &stVPortAttr);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

HI_S32 MSG_VPROC_ExtVportSetAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_SetExtPortAttr(VpssHdl, VPortHdl, (HI_MAPI_EXT_VPORT_ATTR_S *)pstMsg->pBody);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

HI_S32 MSG_VPROC_ExtVportGetAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_MAPI_EXT_VPORT_ATTR_S stExtVPortAttr;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_GetExtPortAttr(VpssHdl, VPortHdl, &stExtVPortAttr);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stExtVPortAttr, sizeof(HI_MAPI_EXT_VPORT_ATTR_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}
static HI_S32 MSG_VPROC_VportStart(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_StartPort(VpssHdl, VPortHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_VportStop(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_StopPort(VpssHdl, VPortHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_SetPortCrop(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_SetPortCrop(VpssHdl, VPortHdl, (VPSS_CROP_INFO_S *)pstMsg->pBody);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;

}

static HI_S32 MSG_VPROC_GetPortCrop(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VPSS_CROP_INFO_S stCropInfo;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = memset_s(&stCropInfo, sizeof(VPSS_CROP_INFO_S), 0x00, sizeof(VPSS_CROP_INFO_S));
    MAPI_CHECK_PRINT(HI_MAPI_MOD_VCAP, "memset_s", s32Ret);

    s32Ret = HI_MAPI_VPROC_GetPortCrop(VpssHdl, VPortHdl, &stCropInfo);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stCropInfo, sizeof(VPSS_CROP_INFO_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_SetAttrEx(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_SetPortAttrEx(VpssHdl, VPortHdl, pstMsg->as32PrivData[0], pstMsg->pBody,
                                         pstMsg->u32BodyLen);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_GetAttrEx(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_GetPortAttrEx(VpssHdl, VPortHdl, pstMsg->as32PrivData[0], pstMsg->pBody,
                                         pstMsg->u32BodyLen);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, pstMsg->pBody, pstMsg->u32BodyLen);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

#ifdef SUPPORT_STITCH
static HI_S32 MSG_VPROC_CreatStitch(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE StitchHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    StitchHdl = GET_DEV_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_CreateStitch(StitchHdl, (HI_MAPI_STITCH_ATTR_S *)pstMsg->pBody);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_DestroyStitch(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE StitchHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    StitchHdl = GET_DEV_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_DestroyStitch(StitchHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_SetStitchAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE StitchHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    StitchHdl = GET_DEV_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_SetStitchAttr(StitchHdl, (HI_MAPI_STITCH_ATTR_S *)pstMsg->pBody);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_GetStitchAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE StitchHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_MAPI_STITCH_ATTR_S stStitchAttr;
    StitchHdl = GET_DEV_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_GetStitchAttr(StitchHdl, &stStitchAttr);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stStitchAttr, sizeof(HI_MAPI_STITCH_ATTR_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_SetStitchPortAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE StitchHdl, StitchPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    StitchHdl = GET_DEV_ID(pstMsg->u32Module);
    StitchPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_SetStitchPortAttr(StitchHdl, StitchPortHdl, (HI_MAPI_STITCH_PORT_ATTR_S *)pstMsg->pBody);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_GetStitchPortAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE StitchHdl, StitchPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_MAPI_STITCH_PORT_ATTR_S stStitchPortAttr;
    StitchHdl = GET_DEV_ID(pstMsg->u32Module);
    StitchPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_GetStitchPortAttr(StitchHdl, StitchPortHdl, &stStitchPortAttr);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stStitchPortAttr, sizeof(HI_MAPI_STITCH_PORT_ATTR_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_StartStitchPort(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE StitchHdl, StitchPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    StitchHdl = GET_DEV_ID(pstMsg->u32Module);
    StitchPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_StartStitchPort(StitchHdl, StitchPortHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_StopStitchPort(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE StitchHdl, StitchPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    StitchHdl = GET_DEV_ID(pstMsg->u32Module);
    StitchPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_StopStitchPort(StitchHdl, StitchPortHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}
#endif

#ifdef SUPPORT_PHOTO_POST_PROCESS
static HI_S32 MSG_VPROC_InitPhoto(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_MAPI_PHOTO_TYPE_E enPhotoType;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);
    enPhotoType = pstMsg->as32PrivData[0];

    s32Ret = HI_MAPI_VPROC_InitPhoto(VpssHdl, VPortHdl, enPhotoType);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_DeinitPhoto(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_DeinitPhoto(VpssHdl, VPortHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_ProcessPhoto(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_PhotoProcess(VpssHdl, VPortHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_GetCurrentPhotoType(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE VpssHdl, VPortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_MAPI_PHOTO_TYPE_E enPhotoType;
    VpssHdl = GET_DEV_ID(pstMsg->u32Module);
    VPortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = HI_MAPI_VPROC_GetCurrentPhotoType(VpssHdl, VPortHdl, &enPhotoType);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &enPhotoType, sizeof(HI_MAPI_PHOTO_TYPE_E));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_SetPhotoAlgCoef(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    PHOTO_ALG_TYPE_E enAlgType = pstMsg->as32PrivData[0];

    s32Ret = HI_MAPI_VPROC_SetPhotoAlgCoef(enAlgType, (PHOTO_ALG_COEF_S *)pstMsg->pBody);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_GetPhotoAlgCoef(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    PHOTO_ALG_COEF_S stPhotoAlgCoef;
    PHOTO_ALG_TYPE_E enAlgType = pstMsg->as32PrivData[0];

    s32Ret = HI_MAPI_VPROC_GetPhotoAlgCoef(enAlgType, &stPhotoAlgCoef);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stPhotoAlgCoef, sizeof(PHOTO_ALG_COEF_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_PHOTOPROCESS_Send(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = VPROC_PhotoProcess_Send(GrpHdl, PortHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_PHOTOPROCESS_GetBNR(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);
    VPROC_PHOTO_DUMP_DATA_S stDumpData;

    s32Ret = memset_s(&stDumpData, sizeof(VPROC_PHOTO_DUMP_DATA_S), 0x00, sizeof(VPROC_PHOTO_DUMP_DATA_S));
    MAPI_CHECK_PRINT(HI_MAPI_MOD_VPROC, "memset_s", s32Ret);

    s32Ret = VPROC_PhotoProcess_GetBNR(GrpHdl, PortHdl, &stDumpData);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stDumpData, sizeof(VPROC_PHOTO_DUMP_DATA_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_PHOTOPROCESS_GetYUV(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);
    VPROC_PHOTO_DUMP_DATA_S stDumpData;

    s32Ret = memset_s(&stDumpData, sizeof(VPROC_PHOTO_DUMP_DATA_S), 0x00, sizeof(VPROC_PHOTO_DUMP_DATA_S));
    MAPI_CHECK_PRINT(HI_MAPI_MOD_VPROC, "memset_s", s32Ret);

    s32Ret = VPROC_PhotoProcess_GetYUV(GrpHdl, PortHdl, &stDumpData);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stDumpData, sizeof(VPROC_PHOTO_DUMP_DATA_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_PHOTOPROCESS_AlgProcess(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl;
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = VPROC_PhotoProcess_AlgProcess(GrpHdl, PortHdl);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}


#endif

static HI_S32 MSG_VPROC_SetDumpYUVAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    MOD_ID_E enMod = pstMsg->as32PrivData[0];
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);

    switch (enMod) {
        case HI_ID_VPSS:
            s32Ret = HI_MAPI_VPROC_SetVpssDumpYUVAttr(GrpHdl, PortHdl, (HI_MAPI_DUMP_YUV_ATTR_S *)pstMsg->pBody);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        case HI_ID_AVS:
            s32Ret = HI_MAPI_VPROC_SetStitchDumpYUVAttr(GrpHdl, PortHdl, (HI_MAPI_DUMP_YUV_ATTR_S *)pstMsg->pBody);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "mod ID %d not support. \n", enMod);
            break;
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_GetDumpYUVAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_MAPI_DUMP_YUV_ATTR_S stDumpYUVAttr;
    MOD_ID_E enMod = pstMsg->as32PrivData[0];
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);

    switch (enMod) {
        case HI_ID_VPSS:
            s32Ret = HI_MAPI_VPROC_GetVpssDumpYUVAttr(GrpHdl, PortHdl, &stDumpYUVAttr);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        case HI_ID_AVS:
            s32Ret = HI_MAPI_VPROC_GetStitchDumpYUVAttr(GrpHdl, PortHdl, &stDumpYUVAttr);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "mod ID %d not support. \n", enMod);
            break;
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stDumpYUVAttr, sizeof(HI_MAPI_DUMP_YUV_ATTR_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_GetChnFrame(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_MAPI_FRAME_DATA_S stYUVFrameData;
    MOD_ID_E enMod = pstMsg->as32PrivData[0];
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);

    s32Ret = memset_s(&stYUVFrameData, sizeof(HI_MAPI_FRAME_DATA_S), 0x00, sizeof(HI_MAPI_FRAME_DATA_S));
    MAPI_CHECK_PRINT(HI_MAPI_MOD_VPROC, "memset_s", s32Ret);
    switch (enMod) {
        case HI_ID_VPSS:
            s32Ret = VPROC_GetChnFrame(GrpHdl, PortHdl, HI_FALSE, &stYUVFrameData);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        case HI_ID_AVS:
            s32Ret = VPROC_GetChnFrame(GrpHdl, PortHdl, HI_TRUE, &stYUVFrameData);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "mod ID %d not support. \n", enMod);
            break;
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stYUVFrameData, sizeof(HI_MAPI_FRAME_DATA_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_ReleaseChnFrame(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    MOD_ID_E enMod = pstMsg->as32PrivData[0];
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);

    switch (enMod) {
        case HI_ID_VPSS:
            s32Ret = VPROC_ReleaseChnFrame(GrpHdl, PortHdl, HI_FALSE, (HI_MAPI_FRAME_DATA_S *)pstMsg->pBody);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        case HI_ID_AVS:
            s32Ret = VPROC_ReleaseChnFrame(GrpHdl, PortHdl, HI_TRUE, (HI_MAPI_FRAME_DATA_S *)pstMsg->pBody);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "mod ID %d not support. \n", enMod);
            break;
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_OSD_SetAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl, OSDHdl;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    MOD_ID_E enMod = pstMsg->as32PrivData[0];
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);
    OSDHdl = pstMsg->as32PrivData[1];

    switch (enMod) {
        case HI_ID_VPSS:
            s32Ret = HI_MAPI_VPROC_SetVpssOSDAttr(GrpHdl, PortHdl, OSDHdl, (HI_MAPI_OSD_ATTR_S *)pstMsg->pBody);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        case HI_ID_AVS:
            s32Ret = HI_MAPI_VPROC_SetStitchOSDAttr(GrpHdl, PortHdl, OSDHdl, (HI_MAPI_OSD_ATTR_S *)pstMsg->pBody);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "mod ID %d not support. \n", enMod);
            break;
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_OSD_GetAttr(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl, OSDHdl;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    HI_MAPI_OSD_ATTR_S stOSDAttr;
    MOD_ID_E enMod = pstMsg->as32PrivData[0];
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);
    OSDHdl = pstMsg->as32PrivData[1];

    switch (enMod) {
        case HI_ID_VPSS:
            s32Ret = HI_MAPI_VPROC_GetVpssOSDAttr(GrpHdl, PortHdl, OSDHdl, &stOSDAttr);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        case HI_ID_AVS:
            s32Ret = HI_MAPI_VPROC_GetStitchOSDAttr(GrpHdl, PortHdl, OSDHdl, &stOSDAttr);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "mod ID %d not support. \n", enMod);
            break;
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &stOSDAttr, sizeof(HI_MAPI_OSD_ATTR_S));
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_OSD_Start(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl, OSDHdl;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    MOD_ID_E enMod = pstMsg->as32PrivData[0];
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);
    OSDHdl = pstMsg->as32PrivData[1];

    switch (enMod) {
        case HI_ID_VPSS:
            s32Ret = HI_MAPI_VPROC_StartVpssOSD(GrpHdl, PortHdl, OSDHdl);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        case HI_ID_AVS:
            s32Ret = HI_MAPI_VPROC_StartStitchOSD(GrpHdl, PortHdl, OSDHdl);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "mod ID %d not support. \n", enMod);
            break;
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}

static HI_S32 MSG_VPROC_OSD_Stop(HI_S32 siId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_HANDLE GrpHdl, PortHdl, OSDHdl;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *respMsg = HI_NULL;
    MOD_ID_E enMod = pstMsg->as32PrivData[0];
    GrpHdl = GET_DEV_ID(pstMsg->u32Module);
    PortHdl = GET_CHN_ID(pstMsg->u32Module);
    OSDHdl = pstMsg->as32PrivData[1];

    switch (enMod) {
        case HI_ID_VPSS:
            s32Ret = HI_MAPI_VPROC_StopVpssOSD(GrpHdl, PortHdl, OSDHdl);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        case HI_ID_AVS:
            s32Ret = HI_MAPI_VPROC_StopStitchOSD(GrpHdl, PortHdl, OSDHdl);

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "Failed : %#x!\n", s32Ret);
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "mod ID %d not support. \n", enMod);
            break;
    }

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, NULL, 0);
    if (respMsg == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_CreateRespMessage fail\n");
    }

    s32Ret = HI_IPCMSG_SendAsync(siId, respMsg, NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "HI_IPCMSG_SendAsync fail,ret:%x\n", s32Ret);
        HI_IPCMSG_DestroyMessage(respMsg);
        return s32Ret;
    }

    HI_IPCMSG_DestroyMessage(respMsg);
    return HI_SUCCESS;
}



static MSG_MODULE_CMD_S g_stModuleCmdTable[] = {
    { MSG_CMD_VPROC_INIT_VPSS,        MSG_VPROC_InitVpss },
    { MSG_CMD_VPROC_DEINIT_VPSS,      MSG_VPROC_DeinitVpss },
    { MSG_CMD_VPROC_BIND_VCAP,        MSG_VPROC_BindVcap },
    { MSG_CMD_VPROC_UNBIND_VCAP,      MSG_VPROC_UnbindVcap },
    { MSG_CMD_VPROC_START_RECV_VCAP,  MSG_VPROC_StartRecvVcapYuv },
    { MSG_CMD_VPROC_STOP_RECV_VCAP,   MSG_VPROC_StopRecvVcapYuv },
    { MSG_CMD_VPROC_SET_VPORT_ATTR,   MSG_VPROC_VportSetAttr },
    { MSG_CMD_VPROC_GET_VPORT_ATTR,   MSG_VPROC_VportGetAttr },
    { MSG_CMD_VPROC_SET_EXT_VPORT_ATTR,   MSG_VPROC_ExtVportSetAttr },
    { MSG_CMD_VPROC_GET_EXT_VPORT_ATTR,   MSG_VPROC_ExtVportGetAttr },
    { MSG_CMD_VPROC_START_VPORT,      MSG_VPROC_VportStart },
    { MSG_CMD_VPROC_STOP_VPORT,       MSG_VPROC_VportStop },
    { MSG_CMD_VPROC_SET_VPORT_ATTREX, MSG_VPROC_SetAttrEx },
    { MSG_CMD_VPROC_GET_VPORT_ATTREX, MSG_VPROC_GetAttrEx },
    { MSG_CMD_VPROC_SET_VPORT_CROP, MSG_VPROC_SetPortCrop },
    { MSG_CMD_VPROC_GET_VPORT_CROP, MSG_VPROC_GetPortCrop },
#ifdef SUPPORT_STITCH
    { MSG_CMD_VPROC_CREAT_STITCH,         MSG_VPROC_CreatStitch },
    { MSG_CMD_VPROC_DESTROY_STITCH,       MSG_VPROC_DestroyStitch },
    { MSG_CMD_VPROC_SET_STITCH_ATTR,      MSG_VPROC_SetStitchAttr },
    { MSG_CMD_VPROC_GET_STITCH_ATTR,      MSG_VPROC_GetStitchAttr },
    { MSG_CMD_VPROC_SET_STITCH_PORT_ATTR, MSG_VPROC_SetStitchPortAttr },
    { MSG_CMD_VPROC_GET_STITCH_PORT_ATTR, MSG_VPROC_GetStitchPortAttr },
    { MSG_CMD_VPROC_START_STITCH_PORT,    MSG_VPROC_StartStitchPort },
    { MSG_CMD_VPROC_STOP_STITCH_PORT,     MSG_VPROC_StopStitchPort },
#endif
#ifdef SUPPORT_PHOTO_POST_PROCESS
    { MSG_CMD_VPROC_INIT_PHOTO,               MSG_VPROC_InitPhoto },
    { MSG_CMD_VPROC_DEINIT_PHOTO,             MSG_VPROC_DeinitPhoto },
    { MSG_CMD_VPROC_PHOTO_PROCESS,            MSG_VPROC_ProcessPhoto },
    { MSG_CMD_VPROC_PHOTO_PROCESS_GETBNR,     MSG_VPROC_PHOTOPROCESS_GetBNR },
    { MSG_CMD_VPROC_PHOTO_PROCESS_GETYUV,     MSG_VPROC_PHOTOPROCESS_GetYUV },
    { MSG_CMD_VPROC_PHOTO_PROCESS_ALGPROCESS, MSG_VPROC_PHOTOPROCESS_AlgProcess },
    { MSG_CMD_VPROC_PHOTO_PROCESS_SEND,       MSG_VPROC_PHOTOPROCESS_Send },
    { MSG_CMD_VPROC_GET_CURRENT_PHOTO_TYPE,   MSG_VPROC_GetCurrentPhotoType },
    { MSG_CMD_VPROC_SET_PHOTO_ALGCOEF,        MSG_VPROC_SetPhotoAlgCoef },
    { MSG_CMD_VPROC_GET_PHOTO_ALGCOEF,        MSG_VPROC_GetPhotoAlgCoef },
#endif
    { MSG_CMD_VPROC_GET_CHN_FRAME,     MSG_VPROC_GetChnFrame },
    { MSG_CMD_VPROC_RELEASE_CHN_FRAME, MSG_VPROC_ReleaseChnFrame },
    { MSG_CMD_VPROC_SET_DUMP_YUV_ATTR, MSG_VPROC_SetDumpYUVAttr },
    { MSG_CMD_VPROC_GET_DUMP_YUV_ATTR, MSG_VPROC_GetDumpYUVAttr },
    { MSG_CMD_VPROC_SET_OSD_ATTR,      MSG_VPROC_OSD_SetAttr },
    { MSG_CMD_VPROC_GET_OSD_ATTR,      MSG_VPROC_OSD_GetAttr },
    { MSG_CMD_VPROC_START_OSD,         MSG_VPROC_OSD_Start },
    { MSG_CMD_VPROC_STOP_OSD,          MSG_VPROC_OSD_Stop },
};

MSG_SERVER_MODULE_S g_stModuleVproc = {
    HI_MAPI_MOD_VPROC,
    "vproc",
    sizeof(g_stModuleCmdTable) / sizeof(MSG_MODULE_CMD_S),
    &g_stModuleCmdTable[0]
};

MSG_SERVER_MODULE_S *MAPI_MSG_GetVProcMod(HI_VOID)
{
    return &g_stModuleVproc;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
