/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    mapi_adec.c
 * @brief  NDK adec server functions
 * @author  HiMobileCam NDK develop team
 * @date       2019/06/19
 */

#include "hi_mapi_adec.h"
#include "hi_type.h"
#include "hi_comm_adec.h"
#include "hi_mapi_comm_define.h"
#include "hi_mapi_adec_define.h"
#include "mapi_adec_inner.h"
#include "mpi_audio.h"
#include "adpt_adec_aac.h"
#include "mapi_comm_inner.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define MAPI_ADEC_DEFAULT_BUF_SIZE 50
static MAPI_ADEC_CONTEXT_S g_stAdecContext = {
    .bAdecInited = HI_FALSE,
    .astAdecChn = {
        {0},
    },
};

HI_S32 MAPI_ADEC_Init()
{
    HI_U32 i;
    HI_S32 s32Ret;

    /* check state */
    if (g_stAdecContext.bAdecInited == HI_TRUE) {
        return HI_SUCCESS;
    }

    /* init chn and datafifo attr */
    s32Ret = memset_s(&g_stAdecContext, sizeof(MAPI_ADEC_CONTEXT_S), 0, sizeof(MAPI_ADEC_CONTEXT_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_AENC, "memset_s", s32Ret);

    for (i = 0; i < HI_MAPI_ADEC_CHN_MAX_NUM; i++) {
        g_stAdecContext.astAdecChn[i].bChnInited = HI_FALSE;
        g_stAdecContext.astAdecChn[i].enAdecFormat = HI_MAPI_AUDIO_FORMAT_BUTT;
        MUTEX_INIT_LOCK(g_stAdecContext.astAdecChn[i].adecFuncLock);
    }

    /* init module  state */
    g_stAdecContext.bAdecInited = HI_TRUE;

    MAPI_DEBUG_TRACE(HI_MAPI_MOD_AENC, "MAPI_AENC_Init!\n");
    return HI_SUCCESS;
}

HI_S32 MAPI_ADEC_Deinit()
{
    HI_U32 i;
    HI_S32 s32Ret;

    /* init chn and datafifo attr */
    s32Ret = memset_s(&g_stAdecContext, sizeof(MAPI_ADEC_CONTEXT_S), 0, sizeof(MAPI_ADEC_CONTEXT_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_AENC, "memset_s", s32Ret);

    for (i = 0; i < HI_MAPI_ADEC_CHN_MAX_NUM; i++) {
        g_stAdecContext.astAdecChn[i].bChnInited = HI_FALSE;
        g_stAdecContext.astAdecChn[i].enAdecFormat = HI_MAPI_AUDIO_FORMAT_BUTT;
        MUTEX_DESTROY(g_stAdecContext.astAdecChn[i].adecFuncLock);
    }

    /* init module  state */
    g_stAdecContext.bAdecInited = HI_FALSE;

    MAPI_DEBUG_TRACE(HI_MAPI_MOD_AENC, "MAPI_AENC_Deinit!\n");
    return HI_SUCCESS;
}

HI_S32 MAPI_Register_AudioDecoder(HI_MAPI_AUDIO_FORMAT_E enAudioFormat)
{
    HI_S32 s32Ret = HI_SUCCESS;

    switch (enAudioFormat) {
        case HI_MAPI_AUDIO_FORMAT_AACLC:
            s32Ret = MAPI_ADEC_RegisterAac();
            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MAPI_ADEC_AacInit  failed, s32Ret:%x\n",s32Ret);
                return s32Ret;
            }

            break;
        /* not need Register Decoder */
        case HI_MAPI_AUDIO_FORMAT_G711A:
        case HI_MAPI_AUDIO_FORMAT_G711U:
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "not support the AudioFormat!\n");
            return HI_MAPI_ADEC_EILLPARAM;
    }

    return HI_SUCCESS;
}


HI_S32 MAPI_UnRegister_AudioDecoder(HI_MAPI_AUDIO_FORMAT_E enAudioFormat)
{
    HI_S32 s32Ret = HI_SUCCESS;

    switch (enAudioFormat) {
        case HI_MAPI_AUDIO_FORMAT_AACLC:
            s32Ret = MAPI_ADEC_UnRegisterAac();

            if (s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MAPI_ADEC_AacDeInit failed, ret:%d\n", s32Ret);
                return s32Ret;
            }

            break;

        /* not need unRegister Decoder */
        case HI_MAPI_AUDIO_FORMAT_G711A:
        case HI_MAPI_AUDIO_FORMAT_G711U:
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "not support the AudioFormat!\n");
            return HI_MAPI_ADEC_EILLPARAM;
    }

    return HI_SUCCESS;
}


HI_S32 HI_MAPI_ADEC_Init(HI_HANDLE AdecHdl, const HI_MAPI_ADEC_ATTR_S* pstAdecAttr)
{
    HI_S32 s32Ret;
    ADEC_CHN AdChn = AdecHdl;
    ADEC_CHN_ATTR_S stAdecChnAttr;
    ADEC_ATTR_AAC_S stAdecAAC;
    ADEC_ATTR_G711_S stAdecG711;

    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAdecAttr);
    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);

    // Adec module init
    s32Ret = MAPI_ADEC_Init();
    if(s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MAPI_ADEC_Init error.\n");
        return s32Ret;
    }

    MUTEX_LOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    if(g_stAdecContext.astAdecChn[AdecHdl].bChnInited) {
        MAPI_WARN_TRACE(HI_MAPI_MOD_ADEC, "adec is Inited, no need init adec repeatedly!.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return HI_SUCCESS;
    }

    s32Ret = MAPI_Register_AudioDecoder(pstAdecAttr->enAdecFormat);
    if(s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MAPI_Register_AudioDecoder error.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return s32Ret;
    }

    stAdecChnAttr.u32BufSize = MAPI_ADEC_DEFAULT_BUF_SIZE;
    stAdecChnAttr.enMode = pstAdecAttr->enMode;

    switch(pstAdecAttr->enAdecFormat) {
        case HI_MAPI_AUDIO_FORMAT_AACLC: {
                stAdecChnAttr.enType = PT_AAC;
                stAdecChnAttr.pValue = &stAdecAAC;
                stAdecAAC.enTransType = AAC_TRANS_TYPE_ADTS;
            }
            break;

        case HI_MAPI_AUDIO_FORMAT_G711A: {
                stAdecChnAttr.enType = PT_G711A;
                stAdecChnAttr.pValue = &stAdecG711;
            }
            break;

        case HI_MAPI_AUDIO_FORMAT_G711U: {
                stAdecChnAttr.enType = PT_G711U;
                stAdecChnAttr.pValue = &stAdecG711;
            }
            break;

        default:
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "Do not support adec type %d \n", pstAdecAttr->enAdecFormat);
            s32Ret = MAPI_UnRegister_AudioDecoder(pstAdecAttr->enAdecFormat);
            if(s32Ret != HI_SUCCESS) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MAPI_UnRegister_AudioDecoder error.\n");
            }
            MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
            return HI_MAPI_ADEC_EILLPARAM;
    }

    s32Ret = HI_MPI_ADEC_CreateChn(AdChn, &stAdecChnAttr);
    if(s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MPI_ADEC_CreateChn error.\n");
        s32Ret = MAPI_UnRegister_AudioDecoder(pstAdecAttr->enAdecFormat);
        if(s32Ret != HI_SUCCESS) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MAPI_UnRegister_AudioDecoder error.\n");
        }
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return s32Ret;
    }

    g_stAdecContext.astAdecChn[AdecHdl].bChnInited = HI_TRUE;
    g_stAdecContext.astAdecChn[AdecHdl].enAdecFormat = pstAdecAttr->enAdecFormat;
    MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    return s32Ret;
}

HI_S32 HI_MAPI_ADEC_Deinit(HI_HANDLE AdecHdl)
{
    ADEC_CHN AdChn = AdecHdl;
    HI_S32 s32Ret;
    HI_U32 i;
    HI_MAPI_AUDIO_FORMAT_E  enAdecFormat = g_stAdecContext.astAdecChn[AdecHdl].enAdecFormat;
    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);

    MUTEX_LOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    if(!g_stAdecContext.astAdecChn[AdecHdl].bChnInited) {
        MAPI_WARN_TRACE(HI_MAPI_MOD_ADEC, "adec has already deinited.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return HI_SUCCESS;
    }

    s32Ret = HI_MPI_ADEC_DestroyChn(AdChn);
    if(s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MPI_ADEC_DestroyChn error.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return s32Ret;
    }

    for(i = 0; i < HI_MAPI_ADEC_CHN_MAX_NUM; i++) {
        if ((g_stAdecContext.astAdecChn[i].bChnInited == HI_TRUE) && (i != AdecHdl)) {
            break;
        }
    }

    // unRegister AudioDecoder need all chn is destroy
    if (i == HI_MAPI_ADEC_CHN_MAX_NUM) {
        s32Ret = MAPI_UnRegister_AudioDecoder(enAdecFormat);
        if(s32Ret != HI_SUCCESS) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MAPI_UnRegister_AudioDecoder error.\n");
            MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
            return s32Ret;
        }
    }

    g_stAdecContext.astAdecChn[AdecHdl].bChnInited = HI_FALSE;
    g_stAdecContext.astAdecChn[AdecHdl].enAdecFormat = HI_MAPI_AUDIO_FORMAT_BUTT;
    MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    // Adec module deinit need all chn is deinit
    if (i == HI_MAPI_ADEC_CHN_MAX_NUM) {
        s32Ret = MAPI_ADEC_Deinit();
        if(s32Ret != HI_SUCCESS) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "MAPI_ADEC_Deinit error.\n");
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_ADEC_SendStream(HI_HANDLE AdecHdl, const AUDIO_STREAM_S* pstAdecStream, HI_BOOL bBlock)
{
    ADEC_CHN AdChn = AdecHdl;
    HI_S32 s32Ret;

    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAdecStream);
    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAdecStream->pStream);
    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);

    MUTEX_LOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    if(!g_stAdecContext.astAdecChn[AdecHdl].bChnInited) {
        MAPI_WARN_TRACE(HI_MAPI_MOD_ADEC, "ADEC  has not been pre inited.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return HI_MAPI_ADEC_ESTATEERR;
    }
    MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    s32Ret = HI_MPI_ADEC_SendStream(AdChn, pstAdecStream, bBlock);
    if(s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MPI_ADEC_SendStream error.\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_ADEC_GetFrame(HI_HANDLE AdecHdl, HI_MAPI_AUDIO_FRAME_INFO_S* pstAudioFrameInfo, HI_BOOL bBlock)
{
    ADEC_CHN AdChn = AdecHdl;
    AUDIO_FRAME_INFO_S stFrmInfo;
    HI_S32 s32Ret;

    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAudioFrameInfo);
    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);

    MUTEX_LOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    if(!g_stAdecContext.astAdecChn[AdecHdl].bChnInited) {
        MAPI_WARN_TRACE(HI_MAPI_MOD_ADEC, "ADEC  has not been pre inited.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return HI_MAPI_ADEC_ESTATEERR;
    }
    MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    s32Ret = memset_s(&stFrmInfo, sizeof(AUDIO_FRAME_INFO_S), 0, sizeof(AUDIO_FRAME_INFO_S));
   MAPI_CHECK_RET(HI_MAPI_MOD_ADEC, "memset_s", s32Ret);

    s32Ret = HI_MPI_ADEC_GetFrame(AdChn, &stFrmInfo, bBlock);
    if(s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MPI_ADEC_GetFrame error.\n");
        return s32Ret;
    }

    pstAudioFrameInfo->u32Id = stFrmInfo.u32Id;
    pstAudioFrameInfo->pstFrame = stFrmInfo.pstFrame;
    s32Ret = memcpy_s(&pstAudioFrameInfo->stAudioFrame, sizeof(AUDIO_FRAME_S), stFrmInfo.pstFrame,
        sizeof(AUDIO_FRAME_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_ADEC, "memcpy_s", s32Ret);


    return s32Ret;
}

HI_S32 HI_MAPI_ADEC_ReleaseFrame(HI_HANDLE AdecHdl, const HI_MAPI_AUDIO_FRAME_INFO_S* pstAudioFrameInfo)
{
    ADEC_CHN AdChn = AdecHdl;
    AUDIO_FRAME_INFO_S  stFrmInfo;
    HI_S32 s32Ret;

    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAudioFrameInfo);
    CHECK_MAPI_ADEC_NULL_PTR_RET(pstAudioFrameInfo->pstFrame);
    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);

    MUTEX_LOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    if(!g_stAdecContext.astAdecChn[AdecHdl].bChnInited) {
        MAPI_WARN_TRACE(HI_MAPI_MOD_ADEC, "ADEC  has not been pre inited.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return HI_MAPI_ADEC_ESTATEERR;
    }

    s32Ret = memset_s(&stFrmInfo, sizeof(AUDIO_FRAME_INFO_S), 0, sizeof(AUDIO_FRAME_INFO_S));
    MAPI_CHECK_RET_UNLOCK(HI_MAPI_MOD_ADEC, "memset_s", s32Ret, g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    stFrmInfo.u32Id = pstAudioFrameInfo->u32Id;
    stFrmInfo.pstFrame = (AUDIO_FRAME_S *)pstAudioFrameInfo->pstFrame;


    s32Ret = HI_MPI_ADEC_ReleaseFrame(AdChn, &stFrmInfo);
    if(s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MPI_ADEC_ReleaseFrame error.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return s32Ret;
    }

    MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    return s32Ret;
}


HI_S32 HI_MAPI_ADEC_SendEndOfStream(HI_HANDLE AdecHdl)
{
    ADEC_CHN AdChn = AdecHdl;
    HI_S32 s32Ret;
    CHECK_MAPI_ADEC_HANDLE_RANGE_RET(AdecHdl);

    MUTEX_LOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    if(!g_stAdecContext.astAdecChn[AdecHdl].bChnInited) {
        MAPI_WARN_TRACE(HI_MAPI_MOD_ADEC, "ADEC  has not been pre inited.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return HI_MAPI_ADEC_ESTATEERR;
    }

    s32Ret = HI_MPI_ADEC_SendEndOfStream(AdChn, HI_TRUE);
    if(s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MPI_ADEC_SendEndOfStream error.\n");
        MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);
        return s32Ret;
    }

    MUTEX_UNLOCK(g_stAdecContext.astAdecChn[AdecHdl].adecFuncLock);

    return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
