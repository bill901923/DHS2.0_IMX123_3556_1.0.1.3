/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    mapi_adec.c
 * @brief  NDK adec client functions
 * @author  HiMobileCam NDK develop team
 * @date      2019/06/19
 */

#include "hi_comm_adec.h"
#include "hi_mapi_comm_define.h"
#include "hi_mapi_adec_define.h"
#include "mapi_adec_inner.h"
#include "mapi_comm_inner.h"
#include "mpi_sys.h"
#include "msg_adec.h"
#include "msg_define.h"
#include "msg_wrapper.h"

#ifdef __cplusplus
#if __cplusplus
     extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 HI_MAPI_ADEC_Init(HI_HANDLE AdecHdl, const HI_MAPI_ADEC_ATTR_S* pstAdecAttr)
{
    HI_MAPI_ADEC_ATTR_S stAdecAttr;
    HI_S32 s32Ret;
    HI_U32 u32ModFd;

    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAdecAttr);
    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);
    CHECK_MAPI_ADEC_CHECK_INIT_RET(MAPI_GetMediaInitStatus());

    /* send ipmsg */
    s32Ret = memcpy_s(&stAdecAttr, sizeof(HI_MAPI_ADEC_ATTR_S), pstAdecAttr, sizeof(HI_MAPI_ADEC_ATTR_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_ADEC, "memcpy_s", s32Ret);
    u32ModFd = MODFD(HI_MAPI_MOD_ADEC, AdecHdl, 0);
    s32Ret = MAPI_SendSync(u32ModFd, MSG_CMD_ADEC_INIT, &stAdecAttr, sizeof(HI_MAPI_ADEC_ATTR_S), HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MSG_CMD_ADEC_INIT fail.\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_ADEC_Deinit(HI_HANDLE AdecHdl)
{
    HI_S32 s32Ret;
    HI_U32 u32ModFd;

    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);
    CHECK_MAPI_ADEC_CHECK_INIT_RET(MAPI_GetMediaInitStatus());

    /* send ipmsg */
    u32ModFd = MODFD(HI_MAPI_MOD_ADEC, AdecHdl, 0);
    s32Ret = MAPI_SendSync(u32ModFd, MSG_CMD_ADEC_DEINIT, HI_NULL, 0, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MSG_CMD_ADEC_DEINIT fail.\n");
        return s32Ret;
    }

    return HI_SUCCESS;

}

HI_S32 HI_MAPI_ADEC_SendStream(HI_HANDLE AdecHdl, const AUDIO_STREAM_S* pstAdecStream, HI_BOOL bBlock)
{
    HI_S32 s32Ret;
    HI_U32 u32ModFd;
    AUDIO_STREAM_S stAdecStream;
    MAPI_PRIV_DATA_S stMsgPriData;

    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);
    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAdecStream);
    CHECK_MAPI_ADEC_CHECK_INIT_RET(MAPI_GetMediaInitStatus());

    s32Ret = memcpy_s(&stAdecStream, sizeof(AUDIO_STREAM_S), pstAdecStream, sizeof(AUDIO_STREAM_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_ADEC, "memcpy_s", s32Ret);
    stMsgPriData.as32PrivData[0] = (HI_S32)bBlock;

    u32ModFd = MODFD(HI_MAPI_MOD_ADEC, AdecHdl, 0);
    s32Ret = MAPI_SendSync(u32ModFd, MSG_CMD_ADEC_SEND_STREAM, &stAdecStream, sizeof(AUDIO_STREAM_S), &stMsgPriData);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MSG_CMD_ADEC_SEND_STREAM fail.\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}
HI_S32 HI_MAPI_ADEC_GetFrame(HI_HANDLE AdecHdl, HI_MAPI_AUDIO_FRAME_INFO_S* pstAudioFrameInfo, HI_BOOL bBlock)
{
    HI_S32 s32Ret;
    HI_U32 u32ModFd;
    HI_MAPI_AUDIO_FRAME_INFO_S stAudioFrameInfo;
    MAPI_PRIV_DATA_S stMsgPriData;

    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);
    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAudioFrameInfo);
    CHECK_MAPI_ADEC_CHECK_INIT_RET(MAPI_GetMediaInitStatus());

    stMsgPriData.as32PrivData[0] = (HI_S32)bBlock;

    u32ModFd = MODFD(HI_MAPI_MOD_ADEC, AdecHdl, 0);
    s32Ret = MAPI_SendSync(u32ModFd, MSG_CMD_ADEC_GET_FRAME, &stAudioFrameInfo, sizeof(HI_MAPI_AUDIO_FRAME_INFO_S),
        &stMsgPriData);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MSG_CMD_ADEC_SEND_STREAM fail.\n");
        return s32Ret;
    }

    s32Ret = memcpy_s(pstAudioFrameInfo, sizeof(HI_MAPI_AUDIO_FRAME_INFO_S), &stAudioFrameInfo,
        sizeof(HI_MAPI_AUDIO_FRAME_INFO_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_ADEC, "memcpy_s", s32Ret);

    pstAudioFrameInfo->stAudioFrame.u64VirAddr[0] = (HI_U8 *)HI_MPI_SYS_Mmap((HI_U64)
        (HI_UL)pstAudioFrameInfo->stAudioFrame.u64VirAddr[0], pstAudioFrameInfo->stAudioFrame.u32Len);
    if (pstAudioFrameInfo->stAudioFrame.u64VirAddr[0] == HI_NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "pstAudioFrameInfo->stAudioFrame.u64VirAddr[0]:0x%p mmap error!\n",
            pstAudioFrameInfo->stAudioFrame.u64VirAddr[0]);
        return HI_FAILURE;
    }

    if (pstAudioFrameInfo->stAudioFrame.enSoundmode == AUDIO_SOUND_MODE_STEREO) {
        pstAudioFrameInfo->stAudioFrame.u64VirAddr[1] = (HI_U8 *)HI_MPI_SYS_Mmap((HI_U64)
            (HI_UL)pstAudioFrameInfo->stAudioFrame.u64VirAddr[1], pstAudioFrameInfo->stAudioFrame.u32Len);
        if (pstAudioFrameInfo->stAudioFrame.u64VirAddr[1] == HI_NULL) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "pstAudioFrameInfo->stAudioFrame.u64VirAddr[1]:0x%p mmap error!\n",
                pstAudioFrameInfo->stAudioFrame.u64VirAddr[1]);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_ADEC_ReleaseFrame(HI_HANDLE AdecHdl, const HI_MAPI_AUDIO_FRAME_INFO_S* pstAudioFrameInfo)
{
    HI_S32 s32Ret;
    HI_U32 u32ModFd;
    HI_MAPI_AUDIO_FRAME_INFO_S stAudioFrameInfo;

    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);
    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAudioFrameInfo);
    CHECK_MAPI_ADEC_CHECK_INIT_RET(MAPI_GetMediaInitStatus());

    s32Ret = memcpy_s(&stAudioFrameInfo, sizeof(HI_MAPI_AUDIO_FRAME_INFO_S) ,pstAudioFrameInfo,
        sizeof(HI_MAPI_AUDIO_FRAME_INFO_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_ADEC, "memcpy_s", s32Ret);

    s32Ret = HI_MPI_SYS_Munmap((HI_VOID *)stAudioFrameInfo.stAudioFrame.u64VirAddr[0],
        stAudioFrameInfo.stAudioFrame.u32Len);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "munmap fail stAudioFrameInfo.stAudioFrame.u64VirAddr[0]:0x%p, s32Ret:0x%x\n",
                       stAudioFrameInfo.stAudioFrame.u64VirAddr[0], s32Ret);
    }

    if (pstAudioFrameInfo->stAudioFrame.enSoundmode == AUDIO_SOUND_MODE_STEREO) {
        s32Ret = HI_MPI_SYS_Munmap((HI_VOID *)stAudioFrameInfo.stAudioFrame.u64VirAddr[1],
            stAudioFrameInfo.stAudioFrame.u32Len);
        if (s32Ret != HI_SUCCESS) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC,
                "munmap fail stAudioFrameInfo.stAudioFrame.u64VirAddr[1]:0x%p, s32Ret:0x%x\n",
                stAudioFrameInfo.stAudioFrame.u64VirAddr[0], s32Ret);
            return HI_FAILURE;
        }
    }

    u32ModFd = MODFD(HI_MAPI_MOD_ADEC, AdecHdl, 0);
    s32Ret = MAPI_SendSync(u32ModFd, MSG_CMD_ADEC_RELEASE_FRAME, &stAudioFrameInfo, sizeof(HI_MAPI_AUDIO_FRAME_INFO_S),
        HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MSG_CMD_ADEC_RELEASE_FRAME fail.\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_ADEC_SendEndOfStream(HI_HANDLE AdecHdl)
{
    HI_S32 s32Ret;
    HI_U32 u32ModFd;

    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);
    CHECK_MAPI_ADEC_CHECK_INIT_RET(MAPI_GetMediaInitStatus());

    u32ModFd = MODFD(HI_MAPI_MOD_ADEC, AdecHdl, 0);
    s32Ret = MAPI_SendSync(u32ModFd, MSG_CMD_ADEC_SEND_ENDOFSTREAM, HI_NULL, 0, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MSG_CMD_ADEC_SEND_ENDOFSTREAM fail.\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
